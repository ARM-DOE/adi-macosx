################################################################################
#
#  COPYRIGHT (C) 2012 Battelle Memorial Institute.  All Rights Reserved.
#
################################################################################
#
#  Author:
#     name:  Sherman Beus
#     phone: (509) 375-2662
#     email: sherman.beus@pnnl.gov
#
################################################################################
#
#  REPOSITORY INFORMATION:
#    $Revision: 12933 $
#    $Author: sbeus $
#    $Date: 2012-02-23 17:28:46 +0000 (Thu, 23 Feb 2012) $
#    $State: Exp $
#
################################################################################

package DBPasswords;

use strict;
use warnings;

use MIME::Base64;
use Digest::MD5;
use Digest::SHA;

################################################################################
# http://blog.gauner.org/blog/2010/12/19/handling-salted-passwords-in-perl/

my %hashlen = (
    'smd5'    => 16,
    'ssha'    => 20,
    'ssha256' => 32,
    'ssha512' => 64,
);

sub plain_md5 {
    my $pw = shift;
    return "{PLAIN-MD5}" . Digest::MD5::md5_hex($pw);
}
 
sub ldap_md5 {
    my $pw = shift;
    return "{LDAP-MD5}" . pad_base64( Digest::MD5::md5_base64($pw) );
}
 
sub smd5 {
    my $pw = shift;
    my $salt = shift || &make_salt();
    return "{SMD5}" . pad_base64( MIME::Base64::encode( Digest::MD5::md5( $pw . $salt ) . $salt, '' ) );
}
 
sub sha {
    my $pw = shift;
    return "{SHA}" . MIME::Base64::encode( Digest::SHA::sha1($pw), '' );
}
 
sub ssha {
    my $pw = shift;
    my $salt = shift || &make_salt();
    return "{SSHA}" . MIME::Base64::encode( Digest::SHA::sha1( $pw . $salt ) . $salt, '' );
}
 
sub sha256 {
    my $pw = shift;
    return "{SHA256}" . MIME::Base64::encode( Digest::SHA::sha256($pw), '' );
}
 
sub ssha256 {
    my $pw = shift;
    my $salt = shift || &make_salt();
    return "{SSHA256}" . MIME::Base64::encode( Digest::SHA::sha256( $pw . $salt ) . $salt, '' );
}
 
sub sha512 {
    my $pw = shift;
    return "{SHA512}" . MIME::Base64::encode( Digest::SHA::sha512($pw), '' );
}
 
sub ssha512 {
    my $pw = shift;
    my $salt = shift || &make_salt();
    return "{SSHA512}" . MIME::Base64::encode( Digest::SHA::sha512( $pw . $salt ) . $salt, '' );
}
 
sub make_pass {
    my $pw     = shift;
    my $scheme = shift;
    my $salt   = shift || &make_salt();
    if ( $scheme eq 'ldap_md5' ) {
        return &ldap_md5($pw);
    }
    elsif ( $scheme eq 'plain_md5' ) {
        return &plain_md5($pw);
    }
    elsif ( $scheme eq 'sha' ) {
        return &sha($pw);
    }
    elsif ( $scheme eq 'sha256' ) {
        return &sha256($pw);
    }
    elsif ( $scheme eq 'sha512' ) {
        return &sha512($pw);
    }
    elsif ( $scheme eq 'smd5' ) {
        return &smd5( $pw, $salt );
    }
    elsif ( $scheme eq 'ssha' ) {
        return &ssha( $pw, $salt );
    }
    elsif ( $scheme eq 'ssha256' ) {
        return &ssha256( $pw, $salt );
    }
    elsif ( $scheme eq 'ssha512' ) {
        return &ssha512( $pw, $salt );
    }
    else {
        return "{CLEARTEXT}" . $pw;
    }
}
 
sub make_salt {
    my $len   = 8 + int( rand(8) );
    my @bytes = ();
    for my $i ( 1 .. $len ) {
        push( @bytes, rand(255) );
    }
    return pack( 'C*', @bytes );
}

sub pad_base64 {
    my $b64_digest = shift;
    while ( length($b64_digest) % 4 ) {
        $b64_digest .= '=';
    }
    return $b64_digest;
}
 
sub split_pass {
    my $pw       = shift;
    my $pwscheme = 'cleartext';
 
    # get use password scheme and remove leading block
    if ( $pw =~ s/^\{([^}]+)\}// ) {
        $pwscheme = lc($1);
 
        # turn - into _ so we can feed pwscheme to make_pass
        $pwscheme =~ s/-/_/g;
    }
 
    # We have 3 major cases:
    # 1 - cleartext pw, return pw and empty salt
    # 2 - hashed pw, no salt
    # 3 - hashed pw with salt
    if ( !$pwscheme || $pwscheme eq 'cleartext' || $pwscheme eq 'plain' ) {
        return ( 'cleartext', $pw, '' );
    }
    elsif ( $pwscheme =~ m/^(plain-md5|ldap-md5|md5|sha|sha256|sha512)$/i ) {
        $pw = MIME::Base64::decode($pw);
        return ( $pwscheme, $pw, '' );
    }
    elsif ( $pwscheme =~ m/^(smd5|ssha|ssha256|ssha512)/ ) {
 
        # now get hashed pass and salt
        # hashlen can be computed by doing
        # $hashlen = length(Digest::*::digest('string'));
        my $hashlen = $hashlen{$pwscheme};
 
        # pwscheme could also specify an encoding
        # like hex or base64, but right now we assume its b64
        $pw = MIME::Base64::decode($pw);
 
        # unpack byte-by-byte, the hash uses the full eight bit of each byte,
        # the salt may do so, too.
        my @tmp  = unpack( 'C*', $pw );
        my $i    = 0;
        my @hash = ();
 
        # the salted hash has the form: $saltedhash.$salt,
        # so the first bytes (# $hashlen) are the hash, the rest
        # is the variable length salt
        while ( $i < $hashlen ) {
            push( @hash, shift(@tmp) );
            $i++;
        }
 
        # as I've said: the rest is the salt
        my @salt = ();
        foreach my $ele (@tmp) {
            push( @salt, $ele );
            $i++;
        }
 
        # pack it again, byte-by-byte
        my $pw   = pack( 'C' . $hashlen, @hash );
        my $salt = pack( 'C*',           @salt );
 
        return ( $pwscheme, $pw, $salt );
    }
}

sub verify_pass {
 
    # cleartext password
    my $pass = shift;
 
    # hashed pw from db
    my $pwentry = shift;
 
    my ( $pwscheme, undef, $salt ) = &split_pass($pwentry);
 
    my $passh = &make_pass( $pass, $pwscheme, $salt );
 
    if ( $pwentry eq $passh ) {
        return 1;
    }
    
    return 0;
}

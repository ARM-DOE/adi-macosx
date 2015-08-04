# ADI
A cross-platform version of ADI that can compile on linux and OSX

# To Install ADI:

- Install the requisite perl modules. In a terminal, do:
  - `cpan JSON::XS`
  - `cpan CGI::Session`
  - If you have not used cpan before, you will be asked to configure it. Accepting all the defaults will work fine.
- Install [Homebrew](http://brew.sh/) if it's not already installed
- Tap the appropriate Homebrew formulas
  - `brew tap homebrew/science`
  - `brew tap ARM-DOE/adi`
- For Python programmers:
  - First, ensure your default version of Python is python 2.7, and has numpy and Cython installed. The easiest way to do this for new users is to install [Anaconda](http://continuum.io/downloads).
  - `brew install adi --with-python`
- For Python non-users:
  - You can skip the Python dependencies and only install the C libraries with:
  - `brew install adi`

# Set Up an Environment for ADI

- Download and untar the [adi_home](https://engineering.arm.gov/~vonderfecht/adi_home.tar.gz) template directory somewhere you want to work from (note: This tarfile contains a directory structure to get you started running ADI quickly. Note that you can also configure the directory structure however you like on your own.)
- Enter the untarred adi_home directory
- Edit `.db_connect` and `env_vars` to replace `/path/to/adi_home` with the actual path to your `adi_home`
copy all the commands in `env_vars` into your bash terminal, to set up the enviornment. Later you may want to add these to your `.bash_profile`.
- To test your installation:
  - go into dev/vap/src/adi_example1
  - run make clean; make If this fails something about the installation has gone wrong
  - run ..\..\bin\adi_example1 -s sbs -f S2 -b 20110401 -e 20110402 -D 2 -R this should complete successfully.

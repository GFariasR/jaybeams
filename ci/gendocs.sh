#!/bin/bash

# Exit on error
set -ev

# Configure git to use my name and email
git config --global user.name "${GIT_NAME?}"
git config --global user.email "${GIT_EMAIL?}"

# ... notice the target directory ...
git clone https://github.com/"${GIT_NAME?}"/jaybeams doc/html

# Kill them all; let Git sort them out.
# ... basically we remove any existing pages and then add everything
# back again.  That way removed pages are also removed from the
# version control, otherwise we would have pages survive forever ...
(cd doc/html && git checkout gh-pages && git rm -qfr .)

# ... call doxygen directly, no need for fancy make rules in this case ...
doxygen doc/Doxyfile

# ... here is where we add all the pages again ...
cd doc/html
git add --all .

# ... we always commit the changes locally, and if there is nothing to
# commit exit successfully ...
git commit -q -m"Automatically generated documentation" || exit 0

exit 0


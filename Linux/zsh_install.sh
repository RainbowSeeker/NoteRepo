#! /usr/bin/env bash

INSTALL_SCRIPTS=install_$RANDOM.sh

sudo DEBIAN_FRONTEND=noninteractive apt-get -y --quiet --no-install-recommends install -y zsh git curl
curl -fsSL https://raw.githubusercontent.com/zimfw/install/master/install.zsh > $INSTALL_SCRIPTS
zsh $INSTALL_SCRIPTS
echo 'skip_global_compinit=1' > ~/.zshenv 
echo -e 'zmodule prompt-pwd\nzmodule gitster' >> ~/.zimrc
rm -f $INSTALL_SCRIPTS
zsh
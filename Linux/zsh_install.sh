#! /usr/bin/env bash

sudo DEBIAN_FRONTEND=noninteractive apt-get -y --quiet --no-install-recommends install -y zsh git curl
sudo chsh -s $(which zsh) $(whoami)
zsh <(curl -sSL https://raw.githubusercontent.com/zimfw/install/master/install.zsh)
echo 'skip_global_compinit=1' > ~/.zshenv 
echo -e 'zmodule prompt-pwd\nzmodule gitster' >> ~/.zimrc
zsh
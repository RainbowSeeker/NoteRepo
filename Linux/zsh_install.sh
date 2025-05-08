#! /usr/bin/env bash
set -e
sudo DEBIAN_FRONTEND=noninteractive apt-get -y --quiet --no-install-recommends install -y zsh git curl exa zip 
sudo chsh -s $(which zsh) $(whoami)
zsh <(curl -sSL https://raw.githubusercontent.com/zimfw/install/master/install.zsh)
echo 'skip_global_compinit=1' > ~/.zshenv 
echo -e 'zmodule prompt-pwd\nzmodule gitster\nzmodule archive\nzmodule exa\nzmodule fzf' >> ~/.zimrc

# Install fzf
curl -fsSL https://raw.githubusercontent.com/junegunn/fzf/master/install | bash -s -- --bin
sudo mv bin/fzf /usr/local/bin/ && rmdir bin
zsh

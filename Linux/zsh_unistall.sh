#! /usr/bin/env bash

sudo chsh -s $(which bash) $(whoami)
rm -rf ~/.zim ~/.zimrc ~/.zshrc
sudo apt purge -y zsh*
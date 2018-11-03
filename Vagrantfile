# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "debian/jessie64"

  config.vm.synced_folder ".", "/vagrant", type: "rsync",
    rsync__exclude: ".git/"

  config.vm.provision "shell", inline: <<-SHELL
    apt-get update
    apt-get install -y g++ make clang-format-4.0 valgrind

    cd /vagrant
    make install
  SHELL
end

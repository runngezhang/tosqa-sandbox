TOSCA_URL = https://github.com/downloads/Nut-Bolt/tosca
TOSCA_HOST = rpi

all:
	# type "make fresh" to set up from a fresh RPi image

fresh: adjust-hostname tosca-disk-mount create-users warn-build-login \
       release-console install-packages install-tosca autorun-tosca \
       set-passwords 
	# installation complete, rebooting now ...
	shutdown -r now

adjust-hostname:
	hostname toscapi
	sed -i s/raspberrypi/toscapi/ /etc/hostname
	sed -i s/raspberrypi/toscapi/ /etc/hosts

tosca-disk-mount:
	mkdir -p /media/TOSCAUSB
	echo "/dev/disk/by-label/TOSCAUSB /media/TOSCAUSB vfat defaults,noatime 0 0" >>/etc/fstab
	echo "/media/TOSCAUSB/tosca-build.img /home/build ext4 defaults,noatime 0 0" >>/etc/fstab

create-users:
	useradd -m -G dialout,plugdev tosca
	useradd -m -G adm,staff,users build
	echo "build ALL=(ALL) NOPASSWD: ALL" >>/etc/sudoers

warn-build-login:
	>~build/.hushlogin
	echo 'echo "Please insert the TOSCAUSB stick and reboot."' >~build/.bash_login
	chown build:build ~build/.hushlogin ~build/.bash_login

release-console:
	sed -i 's/console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 //' /boot/cmdline.txt
	sed -i 's/T0:23:respawn:.sbin.getty -L ttyAMA0/#&/' /etc/inittab
	kill -1 1
	echo "kernel.sysrq=0" >>/etc/sysctl.conf

install-packages:
	apt-get update
	# TODO: several of these installs should be moved to the build section
	apt-get install -y bison flex gperf texinfo gawk libtool automake m4 \
	                   libncurses5-dev cvs git rsync mtools tmux screen \
	                   nginx libuuid1 luarocks vim libexpat1-dev

install-tosca:
	wget -N $(TOSCA_URL)/$(TOSCA_HOST)-tosca-home.tgz
	tar xfz $(TOSCA_HOST)-tosca-home.tgz -C /home
	wget -N $(TOSCA_URL)/$(TOSCA_HOST)-tosca-tools.tgz
	tar xfz $(TOSCA_HOST)-tosca-tools.tgz -C /usr/local

autorun-tosca:
	echo '@reboot if [ -x bin/onboot ]; then bin/onboot; fi' | crontab -u tosca -

set-passwords:
	passwd tosca
	passwd build

#
# Copyright (C) 2009 Volker Thoms <unconnected@gmx.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-softpwm
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define KernelPackage/$(PKG_NAME)
  SUBMENU:=other modules
  TITLE:=Software PWM on GPIO
  DEPENDS:=@GPIO_SUPPORT 
  FILES:=$(PKG_BUILD_DIR)/gpio-softpwm.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=
  AUTOLOAD:=$(call AutoLoad,99,gpio-softpwm)
endef

define KernelPackage/$(PKG_NAME)/description
 Kernel module for emulating PWM on GPIO.
endef

EXTRA_KCONFIG:= \
	CONFIG_GPIO_SOFTPWM=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,$(PKG_NAME)))

#
# Copyright (C) 2009 Volker Thoms <unconnected@gmx.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=can-modules
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/packages

include $(INCLUDE_DIR)/package.mk

CAN_MENU:=CAN Support

define KernelPackage/can-dev
  SUBMENU:=$(CAN_MENU)
  TITLE:=CAN device support
  DEPENDS:=@LINUX_2_6
  KCONFIG:=
  FILES:=$(LINUX_DIR)/drivers/net/can/can-dev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,can-dev)
endef

define KernelPackage/can-dev/description
 CAN bus device driver
endef

EXTRA_KCONFIG:= \
	CONFIG_CAN_DEV=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
endef

define Build/Compile
endef

$(eval $(call KernelPackage,can-dev))

define KernelPackage/mcp251x
  SUBMENU:=$(CAN_MENU)
  TITLE:=MCP251x support
  DEPENDS:=@LINUX_2_6 +kmod-can-dev
  KCONFIG:=
  FILES:=$(LINUX_DIR)/drivers/net/can/mcp251x.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,91,mcp251x)
endef

define KernelPackage/mcp251x/description
 CAN bus driver for Microchip 251x CAN Controller with SPI Interface
endef

EXTRA_KCONFIG:= \
	CONFIG_MCP251x=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
endef

define Build/Compile
endef

$(eval $(call KernelPackage,mcp251x))

define KernelPackage/vcan
  SUBMENU:=$(CAN_MENU)
  TITLE:=vcan support
  DEPENDS:=@LINUX_2_6 +kmod-can-dev
  KCONFIG:=
  FILES:=$(LINUX_DIR)/drivers/net/can/vcan.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,91,vcan)
endef

define KernelPackage/vcan/description
 CAN bus driver for virtual can interface (like loopback for inet)
endef

EXTRA_KCONFIG:= \
	CONFIG_VCAN=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
endef

define Build/Compile
endef

$(eval $(call KernelPackage,vcan))

define KernelPackage/slcan
  SUBMENU:=$(CAN_MENU)
  TITLE:=slcan support
  DEPENDS:=@LINUX_2_6 +kmod-can-dev
  KCONFIG:=
  FILES:=$(LINUX_DIR)/drivers/net/can/slcan.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,91,slcan)
endef

define KernelPackage/vcan/description
 CAN bus driver for serial line attached CAN controller
endef

EXTRA_KCONFIG:= \
	CONFIG_SLCAN=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
endef

define Build/Compile
endef

$(eval $(call KernelPackage,slcan))


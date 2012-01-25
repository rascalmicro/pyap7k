#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=can-utils
PKG_REV:=1070
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_MIRROR:=0 
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_URL:=svn://svn.berlios.de/socketcan/trunk/
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)/trunk/can-utils
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=socketcan project can-utils
  DEPENDS:=@LINUX_2_6
endef

MAKE_OPTS:= \
	PATH="$(TARGET_PATH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)/trunk/can-utils"

define Build/Compile
	$(MAKE)	-C $(PKG_BUILD_DIR)/trunk/can-utils/can-utils $(MAKE_OPTS)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/asc2log $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/bcmserver $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/canbusload $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/candump $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/cangen $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/canlogserver $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/canplayer $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/cansend $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/cansniffer $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/isotpdump $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/isotprecv $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/isotpsend $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/isotpsniffer $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/isotptun $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/log2asc $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/log2long $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/slcan_attach $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/slcand $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/can-utils/can-utils/slcanpty $(1)/usr/bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

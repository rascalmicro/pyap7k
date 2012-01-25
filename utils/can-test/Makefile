#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=can-test
PKG_REV:=1070
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_MIRROR:=0 
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_URL:=svn://svn.berlios.de/socketcan/trunk/
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)/trunk/test
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=socketcan project can testting utilities
  DEPENDS:=@LINUX_2_6
endef

MAKE_OPTS:= \
	PATH="$(TARGET_PATH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)/trunk/test"

define Build/Compile
	$(MAKE)	-C $(PKG_BUILD_DIR)/trunk/test/test $(MAKE_OPTS)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/canecho $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-cycle $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-dump $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-filter $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-rtr $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-rx-sendto $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-single $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-throttle $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-tx_read $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-bcm-tx-sendto $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-err $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-filter-master $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-filter-server $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-packet $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-proc $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-raw $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-raw-filter $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/trunk/test/test/tst-raw-sendto $(1)/usr/bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

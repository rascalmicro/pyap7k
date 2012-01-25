#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=interfacetest
PKG_RELEASE:=1
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Tools for testing spi and i2c
  DEPENDS:=@GPIO_SUPPORT
endef

define Package/$(PKG_NAME)/description
	Tools for testing the SPI and I2C bus with a client testboard.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDLAGS) \
		-o $(PKG_BUILD_DIR)/spitest $(PKG_BUILD_DIR)/spitest.c
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDLAGS) \
		-o $(PKG_BUILD_DIR)/i2ctest $(PKG_BUILD_DIR)/i2ctest.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spitest $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/i2ctest $(1)/usr/bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

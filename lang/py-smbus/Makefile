#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=py-smbus
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
-include $(if $(DUMP),,$(STAGING_DIR)/mk/python-package.mk)

define Package/py-smbus
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=SMBus bindings for python
  DEPENDS:=+python-mini @LINUX_2_6
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr" \
	)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)

	$(INSTALL_DIR) $(1)/pyap7k/$(PKG_NAME)/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/bin/*.py \
		$(1)/pyap7k/$(PKG_NAME)
endef

define PyPackage/py-smbus/filespec
+|$(PYTHON_PKG_DIR)/smbus.so
endef

#$(eval $(call PyPackage,py-smbus))
$(eval $(call BuildPackage,$(PKG_NAME)))

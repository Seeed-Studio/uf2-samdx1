CHIP_FAMILY = samd11
CHIP_VARIANT = SAMD11D14AS

# Platform specific definitions
ifeq ($(PLAT),nRF54L15)
COMMON_FLAGS += -DNRF54L15
else ifeq ($(PLAT),MG24)
COMMON_FLAGS += -DMG24
else
$(error Must define PLAT as either NRF54L15 or MG24)
endif

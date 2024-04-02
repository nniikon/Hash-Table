include config.mk
 
all:
	@$(MAKE) -C ./logs/
	@$(MAKE) -C ./list/source/

clean:
	@rm -rf $(BUILD_DIR)

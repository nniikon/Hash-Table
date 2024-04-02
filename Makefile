include config.mk
 
all:
	@$(MAKE) -C ./logs/
	@$(MAKE) -C ./list/source/
	@$(MAKE) -C ./hash_table/

clean:
	@rm -rf $(BUILD_DIR)

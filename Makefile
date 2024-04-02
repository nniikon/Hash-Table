include config.mk
 
all:
	@$(MAKE) -C ./logs/
	@$(MAKE) -C ./list/source/
	@$(MAKE) -C ./hash_table/
	@$(GXX) main.cpp $(CFLAGS) -c -o $(BUILD_DIR)/main.o

clean:
	@rm -rf $(BUILD_DIR)

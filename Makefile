include config.mk
 
all:
	@$(MAKE) -C ./logs/
	@$(MAKE) -C ./list/source/
	@$(MAKE) -C ./hash_table/
	@$(MAKE) -C ./file_to_buffer/
	@$(MAKE) -C ./hash_functions/
	@$(GXX) main.cpp $(CFLAGS) -c -o $(BUILD_DIR)/main.o
	@$(GXX) $(CFLAGS) -no-pie -o $(BUILD_DIR)/$(EXEC_NAME) $(BUILD_DIR)/*.o

run:
	$(BUILD_DIR)/$(EXEC_NAME)

clean:
	@rm -rf $(BUILD_DIR)

global HashCRC32_asm

;;==============================================================================
;; HashCRC32 - optimized hash function, handles only up to 16 symbols
;;
;; Input:
;;      rdi - string
;;      rdi - length
;;
;; Returns:
;;      rax - hash
;;
;;==============================================================================
HashCRC32_asm:
        mov eax, 0xDEADDEAD
        jmp      [.table + rsi*8]
.table:
        dq  .left_1
        dq  .left_1
        dq  .left_2
        dq  .left_3
        dq  .left_4
        dq  .left_5
        dq  .left_6
        dq  .left_7
        dq  .left_8
        dq  .left_9
        dq  .left_10
        dq  .left_11
        dq  .left_12
        dq  .left_13
        dq  .left_14
        dq  .left_15
        dq  .left_16
.left_16:
        crc32   rax, qword [rdi + 8]
        crc32   rax, qword [rdi]
        ret
.left_15:
        crc32   eax, byte [rdi+14]
.left_14:
        crc32   eax,  word [rdi+12]
        crc32   eax, dword [rdi+8]
        crc32   rax, qword [rdi]
        ret
.left_13:
        crc32   eax, byte [rdi+12]
.left_12:
        crc32   eax, dword [rdi+8]
        crc32   rax, qword [rdi]
        ret
.left_11:
        crc32   eax, byte [rdi+10]
.left_10:
        crc32   eax,  word [rdi+8]
        crc32   rax, qword [rdi]
        ret
.left_9:
        crc32   eax, byte [rdi+8]
.left_8:
        crc32   rax, qword [rdi]
        ret
.left_7:
        crc32   eax, byte  [rdi + 6]
.left_6:
        crc32   eax, word  [rdi + 4]
        crc32   eax, dword [rdi]
        ret
.left_5:
        crc32   eax,  byte [rdi + 4]
.left_4:
        crc32   eax, dword [rdi]
        ret
.left_3:
        crc32   eax, word [rdi]
        crc32   eax, byte [rdi + 2]
        ret
.left_2:
        crc32   eax, word [rdi]
        ret
.left_1:
        crc32   eax, byte [rdi]
        ret

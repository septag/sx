; u8  c89atomic_compare_and_swap_8 (u8*  dst, u8  expected, u8  desired)
; u16 c89atomic_compare_and_swap_16(u16* dst, u16 expected, u16 desired)
; u32 c89atomic_compare_and_swap_32(u32* dst, u32 expected, u32 desired)
; u64 c89atomic_compare_and_swap_64(u64* dst, u64 expected, u64 desired)
;
; RCX = dst
; RDX = expected
; R8  = desired

GLOBAL c89atomic_compare_and_swap_8
GLOBAL c89atomic_compare_and_swap_16
GLOBAL c89atomic_compare_and_swap_32
GLOBAL c89atomic_compare_and_swap_64

SECTION .text

c89atomic_compare_and_swap_8:
    mov al, dl
    lock cmpxchg [rcx], r8b
    ret

c89atomic_compare_and_swap_16:
    mov ax, dx
    lock cmpxchg [rcx], r8w
    ret

c89atomic_compare_and_swap_32:
    mov eax, edx
    lock cmpxchg [rcx], r8d
    ret
    
c89atomic_compare_and_swap_64:
    mov rax, rdx
    lock cmpxchg [rcx], r8
    ret
    
section .text
global distance_kernel_asm

distance_kernel_asm:
    push rbp
    mov rbp, rsp
    
    push rbx
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    
    mov r12d, ecx          
    mov r13, rdx           
    mov r14, r8            
    mov r15, r9            
    mov rsi, [rbp+48]     
    mov rdi, [rbp+56]     


    xor ebx, ebx           

    mov eax, r12d
    shr eax, 2            
    jz .scalar_loop        


.vector_loop:
    movups xmm0, [r13 + rbx*4]    
    movups xmm1, [r14 + rbx*4]   
    movups xmm2, [r15 + rbx*4]   
    movups xmm3, [rsi + rbx*4]    

    subps xmm1, xmm0             
    mulps xmm1, xmm1             

    subps xmm3, xmm2             
    mulps xmm3, xmm3             

    addps xmm1, xmm3          
    sqrtps xmm1, xmm1            

    movups [rdi + rbx*4], xmm1  

    add ebx, 4                  
    cmp ebx, eax                
    jl .vector_loop

.scalar_loop:
    cmp ebx, r12d              
    jge .done

    movss xmm0, [r13 + rbx*4]  
    movss xmm1, [r14 + rbx*4]    
    movss xmm2, [r15 + rbx*4]    
    movss xmm3, [rsi + rbx*4]    

    subss xmm1, xmm0             
    subss xmm3, xmm2            
    
    mulss xmm1, xmm1           
    mulss xmm3, xmm3            
    
    addss xmm1, xmm3             
    sqrtss xmm1, xmm1            

    movss [rdi + rbx*4], xmm1    

    inc ebx
    jmp .scalar_loop

.done:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbx
    pop rbp
    ret
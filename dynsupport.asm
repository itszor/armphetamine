; consider experimental & extremely broken.

extern processor_prefetchabort
extern processor_dataabort
extern memory_virtualtophysical

%include "structsupport.inc"

        struc tlbentry
.virtual:   resd 1
.physical:  resd 1
.mask:      resd 1
.modestamp: resd 1
.wrbyte:    resd 1
.wrsbyte:   resd 1
.wrhalf:    resd 1
.wrshalf:   resd 1
.wrword:    resd 1
.rdbyte:    resd 1
.rdsbyte:   resd 1
.rdhalf:    resd 1
.rdshalf:   resd 1
.rdword:    resd 1
        endstruc

; arg0: memoryinfo*
; arg1: address
; arg2: tlb entry

; duplicating this functionality is stupid.
global dynsupport_readdataword:function

dynsupport_readdataword:
        push esi
        push edi
        push ebx
        mov eax,[esp+16]  ; eax contains memoryinfo*
        mov ecx,[esp+20]  ; ecx contains address
        mov edx,[esp+24]  ; edx contains tlb entry

        test byte [ebp+37],1

        mov ebx,[edx+tlbentry.modestamp]
        cmp ebx,[eax+meminfo_currentmode]
        je tlbmiss
        
        not esi
        and ecx,esi
        or ecx,[edx+tlbentry.physical]

        mov ebx,ecx
        mov esi,[edx+tlbentry.mask]
        and ebx,esi
        cmp ebx,[edx+tlbentry.virtual]
        je tlbmiss

        push ecx  ; phys addr
        push eax  ; meminfo
        call [edx+tlbentry.rdword]
        add esi,8
        pop ebx
        pop edi
        pop esi
        rts
        
tlbmiss:
        mov esi,eax  ; memoryinfo
        mov edi,ebx  ; physical address
        push edx
        push ecx
        push eax
        call memory_virtualtophysical
        add esp,12
        push esi
        push edi
        call [edx+tlbentry.rdword]
        add esp,8

        pop ebx
        pop edi
        pop esi
        ret
.end:


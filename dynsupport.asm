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

global dynsupport_readdataword:function

dynsupport_readdataword:
        pop eax  ; eax contains memoryinfo*
        pop ecx  ; ecx contains address
        pop edx  ; edx contains tlb entry

        mov ebx,[edx+tlbentry.modestamp]
        cmp ebx,[eax+meminfo_currentmode]
        je tlbmiss

        mov ebx,ecx
        mov esi,[edx+tlbentry.mask]
        and ebx,esi
        cmp ebx,[edx+tlbentry.virtual]
        je tlbmiss
        
        not esi
        and ecx,esi
        or ecx,[edx+tlbentry.physical]

        push ecx  ; phys addr
        push eax  ; meminfo
        call [edx+tlbentry.rdword]
        add esi,8
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
        ret
.end:


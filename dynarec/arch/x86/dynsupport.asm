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

%macro readbank 1
global dynsupport_readbank%1:function
dynsupport_readbank%1
        mov eax,[esp+4]  ; meminfo* mem
        mov ecx,[esp+8]  ; phys address
        mov edx,[eax+meminfo_bank%1]
        and ecx,0xfffffc
        mov eax,[edx+ecx]
        xor ecx,ecx
        ret
%endmacro

        readbank 0
        readbank 1
        readbank 2
        readbank 3

%macro writebank 1
global dynsupport_writebank%1:function
dynsupport_writebank%1
        mov eax,[esp+4]   ; meminfo* mem
        mov ecx,[esp+12]  ; phys address
        and ecx,0xfffffc
        mov edx,[eax+meminfo_bank%1]
        mov eax,[esp+8]   ; word to write
        mov [edx+ecx],eax
        xor ecx,ecx
        ret
%endmacro

        writebank 0
        writebank 1
        writebank 2
        writebank 3

; duplicating this functionality is stupid, but hey

global dynsupport_readdataword:function
dynsupport_readdataword:
        push esi
        push edi
        push ebx
        mov eax,[esp+16]  ; eax contains memoryinfo*
        mov ecx,[esp+20]  ; ecx contains address
        mov edx,[esp+24]  ; edx contains tlb entry

        mov ebx,[edx+tlbentry.modestamp]
        cmp ebx,[eax+meminfo_currentmode]
        je rdw_tlbmiss
        
        mov ebx,ecx
        mov esi,[edx+tlbentry.mask]
        and ebx,esi
        cmp ebx,[edx+tlbentry.virtual]
        je rdw_tlbmiss

        not esi
        and ecx,esi
        or ecx,[edx+tlbentry.physical]

        push ecx  ; phys addr
        push eax  ; meminfo
        call [edx+tlbentry.rdword]
        add esp,8
        pop ebx
        pop edi
        pop esi
        ret
        
rdw_tlbmiss:
        mov esi,eax  ; memoryinfo
        mov edi,ebx  ; physical address
        push edx
        push ecx
        push eax
        call memory_virtualtophysical
        add esp,12
        push edi  ; phys address
        push esi  ; meminfo
        call [edx+tlbentry.rdword]
        add esp,8
        pop ebx
        pop edi
        pop esi
        ret

global dynsupport_writeword:function
dynsupport_writeword:
        push esi
        push edi
        push ebx
        mov eax,[esp+16]  ; eax contains memoryinfo*
        mov ecx,[esp+20]  ; ecx contains address
        mov edx,[esp+28]  ; edx contains tlb entry

        mov esi,[edx+tlbentry.modestamp]
        cmp esi,[eax+meminfo_currentmode]
        je ww_tlbmiss
        
        mov esi,[edx+tlbentry.mask]
        and edi,esi
        cmp edi,[edx+tlbentry.virtual]
        je ww_tlbmiss

        not esi
        and ecx,esi
        or ecx,[edx+tlbentry.physical]

        mov ebx,[esp+24]  ; ebx contains word to write

        push ebx  ; data
        push ecx  ; phys addr
        push eax  ; meminfo
        call [edx+tlbentry.wrword]
        add esp,12
        pop ebx
        pop edi
        pop esi
        ret
        
ww_tlbmiss:
        mov esi,eax  ; memoryinfo
        mov edi,ecx  ; physical address
        push edx  ; tlb
        push ecx  ; phys address
        push eax  ; memoryinfo
        call memory_virtualtophysical
        add esp,12
        mov ebx,[esp+24]  ; ebx contains word to write
        push ebx  ; data
        push edi  ; phys address
        push esi  ; meminfo
        call [edx+tlbentry.wrword]
        add esp,12
        pop ebx
        pop edi
        pop esi
        ret
.end:


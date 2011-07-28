; amxplugin.asm - a rewrite of amxplugin.cpp in asm
; Copyright (c) 2011 Zeex

	global _PluginInit
	global _amx_Align16
	global _amx_Align32
	global _amx_Align64
	global _amx_Allot
	global _amx_Callback
	global _amx_Cleanup
	global _amx_Clone
	global _amx_Exec
	global _amx_FindNative
	global _amx_FindPublic
	global _amx_FindPubVar
	global _amx_FindTagId
	global _amx_Flags
	global _amx_GetAddr
	global _amx_GetNative
	global _amx_GetPublic
	global _amx_GetPubVar
	global _amx_GetString
	global _amx_GetTag
	global _amx_GetUserData
	global _amx_Init	
	global _amx_InitJIT	
	global _amx_MemInfo						
	global _amx_NameLength				
	global _amx_NativeInfo				
	global _amx_NumNatives	
	global _amx_NumPublics							
	global _amx_NumPubVars				
	global _amx_NumTags				
	global _amx_Push				
	global _amx_PushArray
	global _amx_PushString	
	global _amx_RaiseError	
	global _amx_Register	
	global _amx_Release				
	global _amx_SetCallback	
	global _amx_SetDebugHook	
	global _amx_SetString	
	global _amx_SetUserData	
	global _amx_StrLen					
	global _amx_UTF8Check				
	global _amx_UTF8Get				
	global _amx_UTF8Len				
	global _amx_UTF8Put
	global _logprintf

	global _plugin_data
	global _amx_exports
	global _logprintf_ptr
		
	section .text 

_PluginInit:
	mov eax, [esp + 4h]      ; ppData
	mov [_plugin_data], eax
	push ebx                 
	mov ebx, [eax]           ; ppData[PLUGIN_DATA_LOGPRINTF]
	mov [_logprintf_ptr], ebx
	mov ebx, [eax + 10h*4]   ; ppData[PLUGIN_DATA_AMX_EXPORTS]
	mov [_amx_exports], ebx  
	pop ebx                  
	ret

_amx_Align16:
	mov eax, [_amx_exports]
	jmp dword [eax + 0*4]
	
_amx_Align32:
	mov eax, [_amx_exports]
	jmp dword [eax + 1*4]	

_amx_Align64:
	mov eax, [_amx_exports]
	jmp dword [eax + 2*4]

_amx_Allot:
	mov eax, [_amx_exports]
	jmp dword [eax + 3*4]
	
_amx_Callback:
	mov eax, [_amx_exports]
	jmp dword [eax + 4*4]
	
_amx_Cleanup:
	mov eax, [_amx_exports]
	jmp dword [eax + 5*4]
	
_amx_Clone:
	mov eax, [_amx_exports]
	jmp dword [eax + 6*4]			
_amx_Exec:
	mov eax, [_amx_exports]
	jmp dword [eax + 7*4]
	
_amx_FindNative:
	mov eax, [_amx_exports]
	jmp dword [eax + 8*4]
	
_amx_FindPublic:
	mov eax, [_amx_exports]
	jmp dword [eax + 9*4]
	
_amx_FindPubVar:
	mov eax, [_amx_exports]
	jmp dword [eax + 10*4]

_amx_FindTagId:
	mov eax, [_amx_exports]
	jmp dword [eax + 11*4]
	
_amx_Flags:
	mov eax, [_amx_exports]
	jmp dword [eax + 12*4]
	
_amx_GetAddr:
	mov eax, [_amx_exports]
	jmp dword [eax + 13*4]
	
_amx_GetNative:
	mov eax, [_amx_exports]
	jmp dword [eax + 14*4]

_amx_GetPublic:
	mov eax, [_amx_exports]
	jmp dword [eax + 15*4]
	
_amx_GetPubVar:
	mov eax, [_amx_exports]
	jmp dword [eax + 16*4]
	
_amx_GetString:
	mov eax, [_amx_exports]
	jmp dword [eax + 17*4]
	
_amx_GetTag:
	mov eax, [_amx_exports]
	jmp dword [eax + 18*4]
	
_amx_GetUserData:
	mov eax, [_amx_exports]
	jmp dword [eax + 19*4]
	
_amx_Init:
	mov eax, [_amx_exports]
	jmp dword [eax + 20*4]
	
_amx_InitJIT:
	mov eax, [_amx_exports]
	jmp dword [eax + 21*4]
	
_amx_MemInfo:
	mov eax, [_amx_exports]
	jmp dword [eax + 22*4]						
	
_amx_NameLength:
	mov eax, [_amx_exports]
	jmp dword [eax + 23*4]			
	
_amx_NativeInfo:
	mov eax, [_amx_exports]
	jmp dword [eax + 24*4]			
	
_amx_NumNatives:
	mov eax, [_amx_exports]
	jmp dword [eax + 25*4]			
	
_amx_NumPublics:
	mov eax, [_amx_exports]
	jmp dword [eax + 26*4]							
	
_amx_NumPubVars:
	mov eax, [_amx_exports]
	jmp dword [eax + 27*4]				
	
_amx_NumTags:
	mov eax, [_amx_exports]
	jmp dword [eax + 28*4]				
	
_amx_Push:
	mov eax, [_amx_exports]
	jmp dword [eax + 29*4]				
	
_amx_PushArray:
	mov eax, [_amx_exports]
	jmp dword [eax + 30*4]				
	
_amx_PushString:
	mov eax, [_amx_exports]
	jmp dword [eax + 31*4]				
	
_amx_RaiseError:
	mov eax, [_amx_exports]
	jmp dword [eax + 32*4]				
	
_amx_Register:
	mov eax, [_amx_exports]
	jmp dword [eax + 33*4]				
	
_amx_Release:
	mov eax, [_amx_exports]
	jmp dword [eax + 34*4]				
	
_amx_SetCallback:
	mov eax, [_amx_exports]
	jmp dword [eax + 35*4]				
	
_amx_SetDebugHook:
	mov eax, [_amx_exports]
	jmp dword [eax + 36*4]				
	
_amx_SetString:
	mov eax, [_amx_exports]
	jmp dword [eax + 37*4]				
	
_amx_SetUserData:
	mov eax, [_amx_exports]
	jmp dword [eax + 38*4]				
	
_amx_StrLen:
	mov eax, [_amx_exports]
	jmp dword [eax + 39*4]				
	
_amx_UTF8Check:
	mov eax, [_amx_exports]
	jmp dword [eax + 40*4]			
	
_amx_UTF8Get:
	mov eax, [_amx_exports]
	jmp dword [eax + 41*4]			
		
_amx_UTF8Len:
	mov eax, [_amx_exports]	
	jmp dword [eax + 42*4]	
				
_amx_UTF8Put:
	mov eax, [_amx_exports]
	jmp dword [eax + 43*4]
										
_logprintf:
	mov eax, _logprintf_ptr
	jmp dword [_logprintf_ptr]	
	ret
	
	section .data
	
_plugin_data:   dd 0		
_amx_exports:   dd 0
_logprintf_ptr: dd 0

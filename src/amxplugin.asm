; amxplugin.asm - a rewrite of amxplugin.cpp in asm
; Copyright (c) 2011 Zeex

	global PluginInit
	global amx_Align16
	global amx_Align32
	global amx_Align64
	global amx_Allot
	global amx_Callback
	global amx_Cleanup
	global amx_Clone
	global amx_Exec
	global amx_FindNative
	global amx_FindPublic
	global amx_FindPubVar
	global amx_FindTagId
	global amx_Flags
	global amx_GetAddr
	global amx_GetNative
	global amx_GetPublic
	global amx_GetPubVar
	global amx_GetString
	global amx_GetTag
	global amx_GetUserData
	global amx_Init	
	global amx_InitJIT	
	global amx_MemInfo						
	global amx_NameLength				
	global amx_NativeInfo				
	global amx_NumNatives	
	global amx_NumPublics							
	global amx_NumPubVars				
	global amx_NumTags				
	global amx_Push				
	global amx_PushArray
	global amx_PushString	
	global amx_RaiseError	
	global amx_Register	
	global amx_Release				
	global amx_SetCallback	
	global amx_SetDebugHook	
	global amx_SetString	
	global amx_SetUserData	
	global amx_StrLen					
	global amx_UTF8Check				
	global amx_UTF8Get				
	global amx_UTF8Len				
	global amx_UTF8Put
	global logprintf

	global ppPluginData
	global pAMXFunctions
		
	section .text 

PluginInit:
	mov eax, [esp + 4h]      
	mov [ppPluginData], eax
	push ebx                 
	mov ebx, [eax + 10h*4]   ; ppPluginData[PLUGIN_DATA_AMX_EXPORTS]
	mov [pAMXFunctions], ebx  
	pop ebx                  
	ret

amx_Align16:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 0*4]
	
amx_Align32:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 1*4]	

amx_Align64:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 2*4]

amx_Allot:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 3*4]
	
amx_Callback:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 4*4]
	
amx_Cleanup:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 5*4]
	
amx_Clone:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 6*4]			
amx_Exec:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 7*4]
	
amx_FindNative:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 8*4]
	
amx_FindPublic:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 9*4]
	
amx_FindPubVar:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 10*4]

amx_FindTagId:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 11*4]
	
amx_Flags:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 12*4]
	
amx_GetAddr:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 13*4]
	
amx_GetNative:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 14*4]

amx_GetPublic:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 15*4]
	
amx_GetPubVar:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 16*4]
	
amx_GetString:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 17*4]
	
amx_GetTag:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 18*4]
	
amx_GetUserData:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 19*4]
	
amx_Init:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 20*4]
	
amx_InitJIT:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 21*4]
	
amx_MemInfo:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 22*4]						
	
amx_NameLength:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 23*4]			
	
amx_NativeInfo:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 24*4]			
	
amx_NumNatives:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 25*4]			
	
amx_NumPublics:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 26*4]							
	
amx_NumPubVars:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 27*4]				
	
amx_NumTags:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 28*4]				
	
amx_Push:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 29*4]				
	
amx_PushArray:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 30*4]				
	
amx_PushString:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 31*4]				
	
amx_RaiseError:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 32*4]				
	
amx_Register:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 33*4]				
	
amx_Release:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 34*4]				
	
amx_SetCallback:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 35*4]				
	
amx_SetDebugHook:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 36*4]				
	
amx_SetString:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 37*4]				
	
amx_SetUserData:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 38*4]				
	
amx_StrLen:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 39*4]				
	
amx_UTF8Check:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 40*4]			
	
amx_UTF8Get:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 41*4]			
		
amx_UTF8Len:
	mov eax, [pAMXFunctions]	
	jmp dword [eax + 42*4]	
				
amx_UTF8Put:
	mov eax, [pAMXFunctions]
	jmp dword [eax + 43*4]
										
logprintf:
	mov eax, [ppPluginData]
	jmp dword [eax + 0*4]	
	ret
	
	section .data
	
ppPluginData:  dd 0		
pAMXFunctions: dd 0

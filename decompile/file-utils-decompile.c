#include "file-utils-decompile.h"


undefined1 _module;
undefined *PTR_vtable_00005048;
undefined *PTR_typeinfo_00005030;
undefined *PTR_~length_error_00005018;
undefined DAT_000052d8;
pointer _Errors;
undefined *PTR__napi_create_error_00005060;
undefined UNK_000030bc;
undefined *PTR____stack_chk_guard_00005058;
undefined DAT_000052e0;
undefined *PTR_~map_00005028;
undefined *PTR_typeinfo_00005038;
undefined *PTR_~runtime_error_00005020;
undefined RunFetchDiskUsage;
undefined *PTR_Wrapper_00005010;
undefined __invoke;
undefined *PTR___Unwind_Resume_00005068;
undefined *PTR___throw_length_error_00005070;
undefined *PTR_logic_error_00005078;
undefined *PTR_runtime_error_00005080;
undefined *PTR_resize_00005088;
undefined *PTR_reserve_00005090;
undefined *PTR_string_00005098;
undefined *PTR_terminate_000050a0;
undefined *PTR_operator.delete[]_000050a8;
undefined *PTR_operator.delete_000050b0;
undefined *PTR_operator.new[]_000050b8;
undefined *PTR_operator.new_000050c0;
undefined *PTR____cxa_allocate_exception_000050c8;
undefined *PTR____cxa_atexit_000050d0;
undefined *PTR____cxa_begin_catch_000050d8;
undefined *PTR____cxa_end_catch_000050e0;
undefined *PTR____cxa_free_exception_000050e8;
undefined *PTR____cxa_throw_000050f0;
undefined *PTR____stack_chk_fail_000050f8;
undefined *PTR__malloc_00005100;
undefined *PTR__memcmp_00005108;
undefined *PTR__memcpy_00005110;
undefined *PTR__napi_close_handle_scope_00005118;
undefined *PTR__napi_create_double_00005120;
undefined *PTR__napi_create_error_00005128;
undefined *PTR__napi_create_external_00005130;
undefined *PTR__napi_create_function_00005138;
undefined *PTR__napi_create_object_00005140;
undefined *PTR__napi_create_reference_00005148;
undefined *PTR__napi_create_string_utf8_00005150;
undefined *PTR__napi_create_symbol_00005158;
undefined *PTR__napi_create_type_error_00005160;
undefined *PTR__napi_define_properties_00005168;
undefined *PTR__napi_delete_reference_00005170;
undefined *PTR__napi_fatal_error_00005178;
undefined *PTR__napi_get_and_clear_last_exception_00005180;
undefined *PTR__napi_get_cb_info_00005188;
undefined *PTR__napi_get_last_error_info_00005190;
undefined *PTR__napi_get_reference_value_00005198;
undefined *PTR__napi_get_undefined_000051a0;
undefined *PTR__napi_get_value_string_utf16_000051a8;
undefined *PTR__napi_get_value_string_utf8_000051b0;
undefined *PTR__napi_is_exception_pending_000051b8;
undefined *PTR__napi_module_register_000051c0;
undefined *PTR__napi_open_handle_scope_000051c8;
undefined *PTR__napi_set_property_000051d0;
undefined *PTR__napi_throw_000051d8;
undefined *PTR__napi_typeof_000051e0;
undefined *PTR__statvfs_000051e8;
undefined *PTR__strcat_000051f0;
undefined *PTR__strcpy_000051f8;
undefined *PTR__strlen_00005200;

// initAll(Napi::Env, Napi::Object)

undefined1  [16] initAll(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  undefined1 auVar1 [16];
  
  GetDiskUsage::Init();
  auVar1._8_8_ = param_3;
  auVar1._0_8_ = param_2;
  return auVar1;
}



// __napi_initAll(napi_env__*, napi_value__*)

napi_value__ * __napi_initAll(napi_env__ *param_1,napi_value__ *param_2)

{
  GetDiskUsage::Init((GetDiskUsage *)param_1,param_1,param_2);
  return param_2;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// _register_binding()

void _register_binding(void)

{
  _napi_module_register(&_module);
  return;
}



// Napi::Error::Fatal(char const*, char const*)

void * Napi::Error::Fatal(char *param_1,char *param_2)

{
  undefined1 auVar1 [16];
  void *pvVar2;
  ulong uVar3;
  ulong uVar4;
  byte bStack_48;
  undefined1 auStack_46 [6];
  ulong uStack_40;
  void *pvStack_38;
  
  _napi_fatal_error(param_1,0xffffffffffffffff,param_2,0xffffffffffffffff);
  ___cxa_begin_catch();
  std::terminate();
  String::Utf16Value();
  if ((bStack_48 & 1) == 0) {
    uStack_40 = (ulong)(bStack_48 >> 1);
  }
  uVar3 = uStack_40 + 1;
  auVar1._8_8_ = 0;
  auVar1._0_8_ = uVar3;
  uVar4 = 0xffffffffffffffff;
  if (SUB168(auVar1 * ZEXT816(4),8) == 0) {
    uVar4 = SUB168(auVar1 * ZEXT816(4),0);
  }
  pvVar2 = operator_new__(uVar4);
  if ((bStack_48 & 1) == 0) {
    _memcpy(pvVar2,auStack_46,uVar3 * 4);
  }
  else {
    _memcpy(pvVar2,pvStack_38,uVar3 * 4);
    operator_delete(pvStack_38);
  }
  return pvVar2;
}



void * ___clang_call_terminate(void)

{
  undefined1 auVar1 [16];
  void *pvVar2;
  ulong uVar3;
  ulong uVar4;
  byte bStack_40;
  undefined1 auStack_3e [6];
  ulong uStack_38;
  void *pvStack_30;
  
  ___cxa_begin_catch();
  std::terminate();
  Napi::String::Utf16Value();
  if ((bStack_40 & 1) == 0) {
    uStack_38 = (ulong)(bStack_40 >> 1);
  }
  uVar3 = uStack_38 + 1;
  auVar1._8_8_ = 0;
  auVar1._0_8_ = uVar3;
  uVar4 = 0xffffffffffffffff;
  if (SUB168(auVar1 * ZEXT816(4),8) == 0) {
    uVar4 = SUB168(auVar1 * ZEXT816(4),0);
  }
  pvVar2 = operator_new__(uVar4);
  if ((bStack_40 & 1) == 0) {
    _memcpy(pvVar2,auStack_3e,uVar3 * 4);
  }
  else {
    _memcpy(pvVar2,pvStack_30,uVar3 * 4);
    operator_delete(pvStack_30);
  }
  return pvVar2;
}



// Utils::convertNapiStringToWCharA(Napi::String)

void * Utils::convertNapiStringToWCharA(void)

{
  undefined1 auVar1 [16];
  void *pvVar2;
  ulong uVar3;
  ulong uVar4;
  byte local_38;
  undefined1 local_36 [6];
  ulong local_30;
  void *local_28;
  
  Napi::String::Utf16Value();
  if ((local_38 & 1) == 0) {
    local_30 = (ulong)(local_38 >> 1);
  }
  uVar3 = local_30 + 1;
  auVar1._8_8_ = 0;
  auVar1._0_8_ = uVar3;
  uVar4 = 0xffffffffffffffff;
  if (SUB168(auVar1 * ZEXT816(4),8) == 0) {
    uVar4 = SUB168(auVar1 * ZEXT816(4),0);
  }
  pvVar2 = operator_new__(uVar4);
  if ((local_38 & 1) == 0) {
    _memcpy(pvVar2,local_36,uVar3 * 4);
  }
  else {
    _memcpy(pvVar2,local_28,uVar3 * 4);
    operator_delete(local_28);
  }
  return pvVar2;
}



// Napi::String::Utf16Value() const

void Napi::String::Utf16Value(void)

{
  int iVar1;
  long lVar2;
  void *pvVar3;
  undefined8 *in_RSI;
  ulong *in_RDI;
  Error local_70 [24];
  byte local_58;
  void *local_48;
  ulong local_40;
  undefined8 local_38;
  ulong uStack_30;
  void *local_28;
  
  iVar1 = _napi_get_value_string_utf16(*in_RSI,in_RSI[1],0,0,&local_40);
  if (iVar1 == 0) {
    local_38 = 0;
    uStack_30 = 0;
    local_28 = (void *)0x0;
    std::u16string::reserve((u16string *)&local_38,local_40 + 1);
    std::u16string::resize((u16string *)&local_38,local_40,L'\0');
    if ((local_38 & 1) == 0) {
      lVar2 = 10;
      pvVar3 = (void *)((long)&local_38 + 2);
    }
    else {
      lVar2 = (local_38 & 0xfffffffffffffffe) - 1;
      pvVar3 = local_28;
    }
    iVar1 = _napi_get_value_string_utf16(*in_RSI,in_RSI[1],pvVar3,lVar2,0);
    if (iVar1 == 0) {
      in_RDI[2] = (ulong)local_28;
      in_RDI[1] = uStack_30;
      *in_RDI = local_38;
    }
    else {
      Error::New(local_70,(napi_env__ *)*in_RSI);
      Error::ThrowAsJavaScriptException(local_70);
      if ((local_58 & 1) != 0) {
        operator_delete(local_48);
      }
      Reference<>::~Reference((Reference<> *)local_70);
      in_RDI[2] = 0;
      in_RDI[1] = 0;
      *in_RDI = 0;
      if ((local_38 & 1) != 0) {
        operator_delete(local_28);
      }
    }
  }
  else {
    Error::New(local_70,(napi_env__ *)*in_RSI);
    Error::ThrowAsJavaScriptException(local_70);
    if ((local_58 & 1) != 0) {
      operator_delete(local_48);
    }
    Reference<>::~Reference((Reference<> *)local_70);
    in_RDI[2] = 0;
    in_RDI[1] = 0;
    *in_RDI = 0;
  }
  return;
}



// Utils::charConcat(char const*, char const*)

char * Utils::charConcat(char *param_1,char *param_2)

{
  size_t sVar1;
  size_t sVar2;
  char *pcVar3;
  
  sVar1 = _strlen(param_1);
  sVar2 = _strlen(param_2);
  pcVar3 = _malloc(sVar1 + 1 + sVar2);
  _strcpy(pcVar3,param_1);
  _strcat(pcVar3,param_2);
  return pcVar3;
}



// Napi::Reference<Napi::Object>::~Reference()

void __thiscall Napi::Reference<>::~Reference(Reference<> *this)

{
  if (*(long *)(this + 8) != 0) {
    if (this[0x10] == (Reference<>)0x0) {
      _napi_delete_reference(*(undefined8 *)this);
    }
    *(undefined8 *)(this + 8) = 0;
  }
  return;
}



// Napi::Error::New(napi_env__*)

Error * __thiscall Napi::Error::New(Error *this,napi_env__ *param_1)

{
  code *pcVar1;
  int iVar2;
  size_t sVar3;
  _Unwind_Exception *exception_object;
  byte *pbVar4;
  undefined8 *local_40;
  undefined8 local_38;
  long local_30;
  char local_21;
  
  local_30 = 0;
  iVar2 = _napi_get_last_error_info(param_1,&local_40);
  if (iVar2 == 0) {
    iVar2 = _napi_is_exception_pending(param_1,&local_21);
    if (iVar2 == 0) {
      if (local_21 == '\0') {
LAB_000015a8:
        pbVar4 = (byte *)"Error in native callback";
        if ((byte *)*local_40 != (byte *)0x0) {
          pbVar4 = (byte *)*local_40;
        }
        sVar3 = _strlen((char *)pbVar4);
        iVar2 = _napi_create_string_utf8(param_1,pbVar4,sVar3,&local_38);
        if (iVar2 != 0) {
          Fatal("Error::New","napi_create_string_utf8");
LAB_000016bb:
          exception_object = (_Unwind_Exception *)Fatal("Error::New","napi_create_error");
          if ((*pbVar4 & 1) != 0) {
            operator_delete(*(void **)(this + 0x28));
          }
          Reference<>::~Reference((Reference<> *)this);
          __Unwind_Resume(exception_object);
                    // WARNING: Does not return
          pcVar1 = (code *)invalidInstructionException();
          (*pcVar1)();
        }
        if ((*(uint *)((long)local_40 + 0x14) < 8) &&
           ((0xccU >> (*(uint *)((long)local_40 + 0x14) & 0x1f) & 1) != 0)) {
          iVar2 = _napi_create_type_error(param_1,0,local_38,&local_30);
        }
        else {
          iVar2 = _napi_create_error(param_1,0,local_38,&local_30);
        }
        if (iVar2 != 0) goto LAB_000016bb;
      }
      else {
        iVar2 = _napi_get_and_clear_last_exception(param_1,&local_30);
        if (iVar2 != 0) {
          Fatal("Error::New","napi_get_and_clear_last_exception");
          goto LAB_000015a8;
        }
      }
      *(napi_env__ **)this = param_1;
      *(undefined8 *)(this + 8) = 0;
      this[0x10] = (Error)0x0;
      *(undefined8 *)(this + 0x28) = 0;
      *(undefined8 *)(this + 0x20) = 0;
      *(undefined8 *)(this + 0x18) = 0;
      if (local_30 == 0) {
        return this;
      }
      iVar2 = _napi_create_reference(param_1,local_30,1,this + 8);
      if (iVar2 == 0) {
        return this;
      }
      goto LAB_00001693;
    }
  }
  else {
    Fatal("Error::New","napi_get_last_error_info");
  }
  Fatal("Error::New","napi_is_exception_pending");
LAB_00001693:
  Fatal("Error::Error","napi_create_reference");
                    // WARNING: Does not return
  pcVar1 = (code *)invalidInstructionException();
  (*pcVar1)();
}



// std::u16string::reserve(unsigned long)

void __thiscall std::u16string::reserve(u16string *this,ulong param_1)

{
  u16string uVar1;
  bool bVar2;
  bool bVar3;
  ulong uVar4;
  u16string *puVar5;
  logic_error *plVar6;
  long lVar7;
  u16string *puVar8;
  ulong uVar9;
  ulong uVar10;
  
  if (0x7fffffffffffffef < param_1) {
                    // WARNING: Subroutine does not return
    std::__basic_string_common<true>::__throw_length_error();
  }
  uVar1 = *this;
  if (((byte)uVar1 & 1) == 0) {
    uVar9 = (ulong)((byte)uVar1 >> 1);
    uVar4 = 10;
  }
  else {
    uVar9 = *(ulong *)(this + 8);
    uVar4 = (*(ulong *)this & 0xfffffffffffffffe) - 1;
  }
  if (param_1 < uVar9) {
    param_1 = uVar9;
  }
  if (param_1 < 0xb) {
    uVar10 = 10;
  }
  else {
    uVar10 = (param_1 + 8 & 0xfffffffffffffff8) - 1;
  }
  if (uVar10 == uVar4) {
    return;
  }
  if (uVar10 == 10) {
    puVar5 = this + 2;
    bVar3 = false;
    puVar8 = *(u16string **)(this + 0x10);
    bVar2 = true;
    if (((byte)uVar1 & 1) == 0) {
LAB_000017ce:
      uVar4 = (ulong)((byte)uVar1 >> 1);
      goto LAB_000017e4;
    }
  }
  else {
    lVar7 = uVar10 + 1;
    if (uVar4 < uVar10) {
      if (lVar7 < 0) {
        plVar6 = (logic_error *)___cxa_allocate_exception(0x10);
        std::logic_error::logic_error
                  (plVar6,"allocator<T>::allocate(size_t n) \'n\' exceeds maximum supported size");
        *(undefined **)plVar6 = PTR_vtable_00005048 + 0x10;
                    // WARNING: Subroutine does not return
        ___cxa_throw(plVar6,PTR_typeinfo_00005030,PTR__length_error_00005018);
      }
      puVar5 = operator_new(lVar7 * 2);
    }
    else {
      if (lVar7 < 0) {
        plVar6 = (logic_error *)___cxa_allocate_exception(0x10);
        std::logic_error::logic_error
                  (plVar6,"allocator<T>::allocate(size_t n) \'n\' exceeds maximum supported size");
        *(undefined **)plVar6 = PTR_vtable_00005048 + 0x10;
                    // WARNING: Subroutine does not return
        ___cxa_throw(plVar6,PTR_typeinfo_00005030,PTR__length_error_00005018);
      }
      puVar5 = operator_new(lVar7 * 2);
    }
    if (((byte)uVar1 & 1) == 0) {
      puVar8 = this + 2;
      bVar3 = true;
      bVar2 = false;
      goto LAB_000017ce;
    }
    puVar8 = *(u16string **)(this + 0x10);
    bVar3 = true;
  }
  uVar4 = *(ulong *)(this + 8);
  bVar2 = true;
LAB_000017e4:
  if (uVar4 != 0xffffffffffffffff) {
    lVar7 = 0;
    do {
      *(undefined2 *)(puVar5 + lVar7 * 2) = *(undefined2 *)(puVar8 + lVar7 * 2);
      lVar7 = lVar7 + 1;
    } while (uVar4 + 1 != lVar7);
  }
  if (bVar2) {
    operator_delete(puVar8);
  }
  if (bVar3) {
    *(ulong *)this = uVar10 + 1 | 1;
    *(ulong *)(this + 8) = uVar9;
    *(u16string **)(this + 0x10) = puVar5;
  }
  else {
    *this = (u16string)((char)uVar9 * '\x02');
  }
  return;
}



// std::u16string::resize(unsigned long, char16_t)

void __thiscall std::u16string::resize(u16string *this,ulong param_1,wchar16 param_2)

{
  u16string uVar1;
  ulong uVar2;
  
  uVar1 = *this;
  if (((byte)uVar1 & 1) == 0) {
    uVar2 = (ulong)((byte)uVar1 >> 1);
  }
  else {
    uVar2 = *(ulong *)(this + 8);
  }
  if (uVar2 < param_1) {
    append(this,param_1 - uVar2,param_2);
    return;
  }
  if (((byte)uVar1 & 1) == 0) {
    *(undefined2 *)(this + param_1 * 2 + 2) = 0;
    *this = (u16string)((char)param_1 * '\x02');
    return;
  }
  *(undefined2 *)(*(long *)(this + 0x10) + param_1 * 2) = 0;
  *(ulong *)(this + 8) = param_1;
  return;
}



// std::u16string::append(unsigned long, char16_t)

u16string * __thiscall std::u16string::append(u16string *this,ulong param_1,wchar16 param_2)

{
  u16string uVar1;
  u16string *puVar2;
  u16string *puVar3;
  ulong uVar4;
  ulong uVar5;
  ulong uVar6;
  long lVar7;
  
  if (param_1 != 0) {
    uVar1 = *this;
    uVar4 = (ulong)(byte)uVar1;
    if (((byte)uVar1 & 1) == 0) {
      uVar6 = (ulong)((byte)uVar1 >> 1);
      uVar5 = 10;
    }
    else {
      uVar4 = *(ulong *)this;
      uVar6 = *(ulong *)(this + 8);
      uVar5 = (uVar4 & 0xfffffffffffffffe) - 1;
    }
    if (uVar5 - uVar6 < param_1) {
      __grow_by(this,uVar5,(uVar6 + param_1) - uVar5,uVar6,uVar6,0,0);
      uVar4 = (ulong)(byte)*this;
    }
    if ((uVar4 & 1) == 0) {
      puVar2 = this + 2;
    }
    else {
      puVar2 = *(u16string **)(this + 0x10);
    }
    puVar3 = puVar2 + uVar6 * 2;
    uVar4 = param_1;
    do {
      *(wchar16 *)puVar3 = param_2;
      puVar3 = puVar3 + 2;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
    lVar7 = uVar6 + param_1;
    if (((byte)*this & 1) == 0) {
      *this = (u16string)((char)lVar7 * '\x02');
    }
    else {
      *(long *)(this + 8) = lVar7;
    }
    *(undefined2 *)(puVar2 + lVar7 * 2) = 0;
  }
  return this;
}



// std::u16string::__grow_by(unsigned long, unsigned long, unsigned long, unsigned long, unsigned
// long, unsigned long)

void __thiscall
std::u16string::__grow_by
          (u16string *this,ulong param_1,ulong param_2,ulong param_3,ulong param_4,ulong param_5,
          ulong param_6)

{
  void *pvVar1;
  ulong uVar2;
  logic_error *this_00;
  ulong uVar3;
  u16string *puVar4;
  
  uVar3 = 0x7fffffffffffffef;
  if (0x7fffffffffffffef - param_1 < param_2) {
                    // WARNING: Subroutine does not return
    std::__basic_string_common<true>::__throw_length_error();
  }
  if (((byte)*this & 1) == 0) {
    puVar4 = this + 2;
  }
  else {
    puVar4 = *(u16string **)(this + 0x10);
  }
  if (param_1 < 0x3fffffffffffffe7) {
    uVar3 = param_2 + param_1;
    if (param_2 + param_1 < param_1 * 2) {
      uVar3 = param_1 * 2;
    }
    if (uVar3 < 0xb) {
      uVar3 = 0xb;
    }
    else {
      uVar3 = uVar3 + 8 & 0xfffffffffffffff8;
      if ((long)uVar3 < 0) {
        this_00 = (logic_error *)___cxa_allocate_exception(0x10);
        std::logic_error::logic_error
                  (this_00,"allocator<T>::allocate(size_t n) \'n\' exceeds maximum supported size");
        *(undefined **)this_00 = PTR_vtable_00005048 + 0x10;
                    // WARNING: Subroutine does not return
        ___cxa_throw(this_00,PTR_typeinfo_00005030,PTR__length_error_00005018);
      }
    }
  }
  pvVar1 = operator_new(uVar3 * 2);
  if (param_4 != 0) {
    uVar2 = 0;
    do {
      *(undefined2 *)((long)pvVar1 + uVar2 * 2) = *(undefined2 *)(puVar4 + uVar2 * 2);
      uVar2 = uVar2 + 1;
    } while (param_4 != uVar2);
  }
  if (param_3 - param_5 != param_4) {
    do {
      *(undefined2 *)((long)pvVar1 + param_4 * 2 + param_6 * 2) =
           *(undefined2 *)(puVar4 + param_4 * 2 + param_5 * 2);
      param_4 = param_4 + 1;
    } while (param_3 - param_5 != param_4);
  }
  if (param_1 != 10) {
    operator_delete(puVar4);
  }
  *(void **)(this + 0x10) = pvVar1;
  *(ulong *)this = uVar3 | 1;
  return;
}



// std::map<std::string, std::string, std::less<std::string >, std::allocator<std::pair<std::string
// const, std::string > > >::~map()

void __thiscall std::map<>::~map(map<> *this)

{
  __tree<>::destroy((__tree<> *)this,*(__tree_node **)(this + 8));
  return;
}



// Errors::Error(std::string)

Errors * __thiscall Errors::Error(Errors *this,string *param_2)

{
  undefined8 uVar1;
  undefined *puVar2;
  
  puVar2 = (undefined *)std::__tree<>::find<>((__tree<> *)&_Errors,param_2);
  if (puVar2 == &DAT_000052d8) {
    *(undefined8 *)(this + 0x10) = *(undefined8 *)(param_2 + 0x10);
    uVar1 = *(undefined8 *)param_2;
    *(undefined8 *)(this + 8) = *(undefined8 *)(param_2 + 8);
    *(undefined8 *)this = uVar1;
    *(undefined8 *)(param_2 + 0x10) = 0;
    *(undefined8 *)(param_2 + 8) = 0;
    *(undefined8 *)param_2 = 0;
  }
  else {
    std::string::string((string *)this,(string *)(puVar2 + 0x38));
  }
  return this;
}



// Errors::throwError(Napi::Env, char const*, char const*)

void Errors::throwError(napi_env__ *param_1,char *param_2,char *param_3)

{
  char *pcVar1;
  size_t sVar2;
  Error local_50 [24];
  byte local_38;
  void *local_28;
  
  pcVar1 = (char *)Utils::charConcat(param_2,param_3);
  sVar2 = _strlen(pcVar1);
  Napi::Error::New<>(local_50,param_1,pcVar1,sVar2,
                     (_func_napi_status_napi_env___ptr_napi_value___ptr_napi_value___ptr_napi_value___ptr_ptr
                      *)PTR__napi_create_error_00005060);
  Napi::Error::ThrowAsJavaScriptException(local_50);
  if ((local_38 & 1) != 0) {
    operator_delete(local_28);
  }
  Napi::Reference<>::~Reference((Reference<> *)local_50);
  return;
}



// std::__tree<std::__value_type<std::string, std::string >, std::__map_value_compare<std::string,
// std::__value_type<std::string, std::string >, std::less<std::string >, true>,
// std::allocator<std::__value_type<std::string, std::string > >
// >::destroy(std::__tree_node<std::__value_type<std::string, std::string >, void*>*)

void __thiscall std::__tree<>::destroy(__tree<> *this,__tree_node *param_1)

{
  if (param_1 != (__tree_node *)0x0) {
    destroy(this,*(__tree_node **)param_1);
    destroy(this,*(__tree_node **)(param_1 + 8));
    if (((byte)param_1[0x38] & 1) != 0) {
      operator_delete(*(void **)(param_1 + 0x48));
    }
    if (((byte)param_1[0x20] & 1) != 0) {
      operator_delete(*(void **)(param_1 + 0x30));
    }
    operator_delete(param_1);
    return;
  }
  return;
}



// Napi::Error Napi::Error::New<Napi::Error>(napi_env__*, char const*, unsigned long, napi_status
// (*)(napi_env__*, napi_value__*, napi_value__*, napi_value__**))

Error * __thiscall
Napi::Error::New<>(Error *this,napi_env__ *param_1,char *param_2,ulong param_3,
                  _func_napi_status_napi_env___ptr_napi_value___ptr_napi_value___ptr_napi_value___ptr_ptr
                  *param_4)

{
  code *pcVar1;
  napi_status nVar2;
  int iVar3;
  undefined3 extraout_var;
  napi_value__ *local_60;
  napi_value__ *local_58;
  Error local_50 [24];
  byte local_38;
  void *local_28;
  
  iVar3 = _napi_create_string_utf8(param_1,param_2,param_3,&local_60);
  if (iVar3 == 0) {
    nVar2 = (*param_4)(param_1,(napi_value__ *)0x0,local_60,&local_58);
    if (CONCAT31(extraout_var,nVar2) == 0) {
      *(napi_env__ **)this = param_1;
      *(undefined8 *)(this + 8) = 0;
      this[0x10] = (Error)0x0;
      *(undefined8 *)(this + 0x28) = 0;
      *(undefined8 *)(this + 0x20) = 0;
      *(undefined8 *)(this + 0x18) = 0;
      if (local_58 == (napi_value__ *)0x0) {
        return this;
      }
      iVar3 = _napi_create_reference(param_1,local_58,1,this + 8);
      if (iVar3 == 0) {
        return this;
      }
      Fatal("Error::Error","napi_create_reference");
                    // WARNING: Does not return
      pcVar1 = (code *)invalidInstructionException();
      (*pcVar1)();
    }
    New(local_50,param_1);
    ThrowAsJavaScriptException(local_50);
  }
  else {
    New(local_50,param_1);
    ThrowAsJavaScriptException(local_50);
  }
  if ((local_38 & 1) != 0) {
    operator_delete(local_28);
  }
  Reference<>::~Reference((Reference<> *)local_50);
  *(undefined8 *)(this + 8) = 0;
  this[0x10] = (Error)0x0;
  *(undefined8 *)this = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x18) = 0;
  return this;
}



// std::__tree_iterator<std::__value_type<std::string, std::string >,
// std::__tree_node<std::__value_type<std::string, std::string >, void*>*, long>
// std::__tree<std::__value_type<std::string, std::string >, std::__map_value_compare<std::string,
// std::__value_type<std::string, std::string >, std::less<std::string >, true>,
// std::allocator<std::__value_type<std::string, std::string > >
// >::__emplace_hint_unique_key_args<std::string, std::pair<std::string const, std::string >
// const&>(std::__tree_const_iterator<std::__value_type<std::string, std::string >,
// std::__tree_node<std::__value_type<std::string, std::string >, void*>*, long>, std::string
// const&, std::pair<std::string const, std::string > const&)

__tree_node_base *
std::__tree<>::__emplace_hint_unique_key_args<>
          (__tree<> *param_1,undefined8 param_2,undefined8 param_3)

{
  __tree_node_base **pp_Var1;
  __tree_node_base *p_Var2;
  __tree_node_base *local_50 [3];
  undefined1 local_38 [8];
  __tree_end_node *local_30;
  
  pp_Var1 = __find_equal<>(param_1,param_2,&local_30,local_38,param_3);
  p_Var2 = *pp_Var1;
  if (*pp_Var1 == (__tree_node_base *)0x0) {
    __construct_node<>((pair *)local_50);
    __insert_node_at(param_1,local_30,pp_Var1,local_50[0]);
    p_Var2 = local_50[0];
  }
  return p_Var2;
}



// std::__tree_node_base<void*>*& std::__tree<std::__value_type<std::string, std::string >,
// std::__map_value_compare<std::string, std::__value_type<std::string, std::string >,
// std::less<std::string >, true>, std::allocator<std::__value_type<std::string, std::string > >
// >::__find_equal<std::string >(std::__tree_const_iterator<std::__value_type<std::string,
// std::string >, std::__tree_node<std::__value_type<std::string, std::string >, void*>*, long>,
// std::__tree_end_node<std::__tree_node_base<void*>*>*&, std::__tree_node_base<void*>*&,
// std::string const&)

__tree_node_base ** __thiscall
std::__tree<>::__find_equal<>
          (__tree<> *this,__tree_node_base *param_2,__tree_end_node **param_3,
          __tree_node_base **param_4,string *param_5)

{
  __tree_node_base _Var1;
  string sVar2;
  int iVar3;
  __tree_node_base **pp_Var4;
  ulong uVar5;
  __tree_node_base *p_Var6;
  ulong uVar7;
  __tree_node_base *p_Var8;
  string *psVar9;
  __tree_node_base *p_Var10;
  __tree_node_base *p_Var11;
  ulong uVar12;
  bool bVar13;
  
  if ((__tree_node_base *)(this + 8) == param_2) {
LAB_00001f76:
    p_Var6 = *(__tree_node_base **)param_2;
    p_Var10 = param_2;
    if (*(__tree_node_base **)this == param_2) {
LAB_00002071:
      if (p_Var6 == (__tree_node_base *)0x0) {
        *param_3 = (__tree_end_node *)param_2;
        return (__tree_node_base **)param_2;
      }
      *param_3 = (__tree_end_node *)p_Var10;
      return (__tree_node_base **)(p_Var10 + 8);
    }
    p_Var11 = p_Var6;
    p_Var8 = param_2;
    if (p_Var6 == (__tree_node_base *)0x0) {
      do {
        p_Var10 = *(__tree_node_base **)(p_Var8 + 0x10);
        bVar13 = *(__tree_node_base **)p_Var10 == p_Var8;
        p_Var8 = p_Var10;
      } while (bVar13);
    }
    else {
      do {
        p_Var10 = p_Var11;
        p_Var11 = *(__tree_node_base **)(p_Var10 + 8);
      } while (*(__tree_node_base **)(p_Var10 + 8) != (__tree_node_base *)0x0);
    }
    sVar2 = *param_5;
    if (((byte)sVar2 & 1) == 0) {
      uVar5 = (ulong)((byte)sVar2 >> 1);
    }
    else {
      uVar5 = *(ulong *)(param_5 + 8);
    }
    _Var1 = p_Var10[0x20];
    if (((byte)_Var1 & 1) == 0) {
      uVar12 = (ulong)((byte)_Var1 >> 1);
    }
    else {
      uVar12 = *(ulong *)(p_Var10 + 0x28);
    }
    uVar7 = uVar12;
    if (uVar5 < uVar12) {
      uVar7 = uVar5;
    }
    if (uVar7 != 0) {
      if (((byte)_Var1 & 1) == 0) {
        p_Var11 = p_Var10 + 0x21;
      }
      else {
        p_Var11 = *(__tree_node_base **)(p_Var10 + 0x30);
      }
      if (((byte)sVar2 & 1) == 0) {
        psVar9 = param_5 + 1;
      }
      else {
        psVar9 = *(string **)(param_5 + 0x10);
      }
      iVar3 = _memcmp(p_Var11,psVar9,uVar7);
      if (iVar3 != 0) {
        if (iVar3 < 0) goto LAB_00002071;
        goto LAB_00002059;
      }
    }
    if (uVar12 < uVar5) goto LAB_00002071;
    goto LAB_00002059;
  }
  _Var1 = param_2[0x20];
  if (((byte)_Var1 & 1) == 0) {
    uVar5 = (ulong)((byte)_Var1 >> 1);
  }
  else {
    uVar5 = *(ulong *)(param_2 + 0x28);
  }
  sVar2 = *param_5;
  if (((byte)sVar2 & 1) == 0) {
    uVar12 = (ulong)((byte)sVar2 >> 1);
  }
  else {
    uVar12 = *(ulong *)(param_5 + 8);
  }
  uVar7 = uVar12;
  if (uVar5 < uVar12) {
    uVar7 = uVar5;
  }
  if (uVar7 == 0) {
LAB_00001f71:
    if (uVar12 < uVar5) goto LAB_00001f76;
    if (uVar7 != 0) {
      if (((byte)_Var1 & 1) == 0) {
        p_Var6 = param_2 + 0x21;
      }
      else {
        p_Var6 = *(__tree_node_base **)(param_2 + 0x30);
      }
      if (((byte)sVar2 & 1) == 0) {
        psVar9 = param_5 + 1;
      }
      else {
        psVar9 = *(string **)(param_5 + 0x10);
      }
      goto LAB_000020a6;
    }
LAB_000020d5:
    if (uVar12 <= uVar5) {
LAB_000020ca:
      *param_3 = (__tree_end_node *)param_2;
      *param_4 = param_2;
      return param_4;
    }
  }
  else {
    if (((byte)sVar2 & 1) == 0) {
      psVar9 = param_5 + 1;
    }
    else {
      psVar9 = *(string **)(param_5 + 0x10);
    }
    if (((byte)_Var1 & 1) == 0) {
      p_Var6 = param_2 + 0x21;
    }
    else {
      p_Var6 = *(__tree_node_base **)(param_2 + 0x30);
    }
    iVar3 = _memcmp(psVar9,p_Var6,uVar7);
    if (iVar3 == 0) goto LAB_00001f71;
    if (iVar3 < 0) goto LAB_00001f76;
LAB_000020a6:
    iVar3 = _memcmp(p_Var6,psVar9,uVar7);
    if (iVar3 == 0) goto LAB_000020d5;
    if (-1 < iVar3) goto LAB_000020ca;
  }
  p_Var6 = *(__tree_node_base **)(param_2 + 8);
  p_Var10 = p_Var6;
  if (p_Var6 == (__tree_node_base *)0x0) {
    p_Var11 = *(__tree_node_base **)(param_2 + 0x10);
    p_Var10 = param_2;
    if (*(__tree_node_base **)p_Var11 != param_2) {
      do {
        p_Var10 = *(__tree_node_base **)(p_Var10 + 0x10);
        p_Var11 = *(__tree_node_base **)(p_Var10 + 0x10);
      } while (*(__tree_node_base **)p_Var11 != p_Var10);
    }
  }
  else {
    do {
      p_Var11 = p_Var10;
      p_Var10 = *(__tree_node_base **)p_Var11;
    } while (*(__tree_node_base **)p_Var11 != (__tree_node_base *)0x0);
  }
  if (p_Var11 != (__tree_node_base *)(this + 8)) {
    _Var1 = p_Var11[0x20];
    if (((byte)_Var1 & 1) == 0) {
      uVar5 = (ulong)((byte)_Var1 >> 1);
    }
    else {
      uVar5 = *(ulong *)(p_Var11 + 0x28);
    }
    uVar7 = uVar12;
    if (uVar5 < uVar12) {
      uVar7 = uVar5;
    }
    if (uVar7 != 0) {
      if (((byte)sVar2 & 1) == 0) {
        psVar9 = param_5 + 1;
      }
      else {
        psVar9 = *(string **)(param_5 + 0x10);
      }
      if (((byte)_Var1 & 1) == 0) {
        p_Var10 = p_Var11 + 0x21;
      }
      else {
        p_Var10 = *(__tree_node_base **)(p_Var11 + 0x30);
      }
      iVar3 = _memcmp(psVar9,p_Var10,uVar7);
      if (iVar3 != 0) {
        if (iVar3 < 0) goto LAB_000021a8;
        goto LAB_00002059;
      }
    }
    if (uVar5 <= uVar12) {
LAB_00002059:
      pp_Var4 = __find_equal<>(this,param_3,param_5);
      return pp_Var4;
    }
  }
LAB_000021a8:
  if (p_Var6 == (__tree_node_base *)0x0) {
    *param_3 = (__tree_end_node *)param_2;
    return (__tree_node_base **)(param_2 + 8);
  }
  *param_3 = (__tree_end_node *)p_Var11;
  return (__tree_node_base **)p_Var11;
}



// std::unique_ptr<std::__tree_node<std::__value_type<std::string, std::string >, void*>,
// std::__tree_node_destructor<std::allocator<std::__tree_node<std::__value_type<std::string,
// std::string >, void*> > > > std::__tree<std::__value_type<std::string, std::string >,
// std::__map_value_compare<std::string, std::__value_type<std::string, std::string >,
// std::less<std::string >, true>, std::allocator<std::__value_type<std::string, std::string > >
// >::__construct_node<std::pair<std::string const, std::string > const&>(std::pair<std::string
// const, std::string > const&)

pair * std::__tree<>::__construct_node<>(pair *param_1)

{
  void *pvVar1;
  pair *in_RDX;
  long in_RSI;
  
  pvVar1 = operator_new(0x50);
  *(void **)param_1 = pvVar1;
  *(long *)(param_1 + 8) = in_RSI + 8;
  param_1[0x10] = (pair)0x0;
  pair<>::pair((pair<> *)((long)pvVar1 + 0x20),in_RDX);
  param_1[0x10] = (pair)0x1;
  return param_1;
}



// std::__tree<std::__value_type<std::string, std::string >, std::__map_value_compare<std::string,
// std::__value_type<std::string, std::string >, std::less<std::string >, true>,
// std::allocator<std::__value_type<std::string, std::string > >
// >::__insert_node_at(std::__tree_end_node<std::__tree_node_base<void*>*>*,
// std::__tree_node_base<void*>*&, std::__tree_node_base<void*>*)

void __thiscall
std::__tree<>::__insert_node_at
          (__tree<> *this,__tree_end_node *param_1,__tree_node_base **param_2,
          __tree_node_base *param_3)

{
  *(undefined8 *)(param_3 + 8) = 0;
  *(undefined8 *)param_3 = 0;
  *(__tree_end_node **)(param_3 + 0x10) = param_1;
  *param_2 = param_3;
  if (**(long **)this != 0) {
    *(long *)this = **(long **)this;
    param_3 = *param_2;
  }
  __tree_balance_after_insert<>(*(__tree_node_base **)(this + 8),param_3);
  *(long *)(this + 0x10) = *(long *)(this + 0x10) + 1;
  return;
}



// std::__tree_node_base<void*>*& std::__tree<std::__value_type<std::string, std::string >,
// std::__map_value_compare<std::string, std::__value_type<std::string, std::string >,
// std::less<std::string >, true>, std::allocator<std::__value_type<std::string, std::string > >
// >::__find_equal<std::string >(std::__tree_end_node<std::__tree_node_base<void*>*>*&, std::string
// const&)

__tree_node_base ** __thiscall
std::__tree<>::__find_equal<>(__tree<> *this,__tree_end_node **param_1,string *param_2)

{
  __tree_node_base _Var1;
  int iVar2;
  __tree_node_base *p_Var3;
  ulong uVar4;
  ulong uVar5;
  __tree_node_base *p_Var6;
  __tree_node_base *local_50;
  string *local_48;
  ulong local_38;
  
  p_Var3 = *(__tree_node_base **)(this + 8);
  local_50 = (__tree_node_base *)(this + 8);
  if (p_Var3 == (__tree_node_base *)0x0) {
    *param_1 = (__tree_end_node *)local_50;
  }
  else {
    if (((byte)*param_2 & 1) == 0) {
      local_48 = param_2 + 1;
      local_38 = (ulong)((byte)*param_2 >> 1);
    }
    else {
      local_38 = *(ulong *)(param_2 + 8);
      local_48 = *(string **)(param_2 + 0x10);
    }
    do {
      while( true ) {
        p_Var6 = p_Var3;
        _Var1 = p_Var6[0x20];
        if (((byte)_Var1 & 1) == 0) {
          uVar5 = (ulong)((byte)_Var1 >> 1);
        }
        else {
          uVar5 = *(ulong *)(p_Var6 + 0x28);
        }
        uVar4 = local_38;
        if (uVar5 < local_38) {
          uVar4 = uVar5;
        }
        if (uVar4 == 0) break;
        if (((byte)_Var1 & 1) == 0) {
          p_Var3 = p_Var6 + 0x21;
        }
        else {
          p_Var3 = *(__tree_node_base **)(p_Var6 + 0x30);
        }
        iVar2 = _memcmp(local_48,p_Var3,uVar4);
        if (iVar2 == 0) break;
        if (-1 < iVar2) {
LAB_000022e9:
          iVar2 = _memcmp(p_Var3,local_48,uVar4);
          if (iVar2 == 0) goto LAB_00002303;
          if (-1 < iVar2) goto LAB_000023a7;
          goto LAB_0000230d;
        }
LAB_00002390:
        p_Var3 = *(__tree_node_base **)p_Var6;
        local_50 = p_Var6;
        if (*(__tree_node_base **)p_Var6 == (__tree_node_base *)0x0) {
          *param_1 = (__tree_end_node *)p_Var6;
          return (__tree_node_base **)p_Var6;
        }
      }
      if (local_38 < uVar5) goto LAB_00002390;
      if (uVar4 != 0) {
        if (((byte)_Var1 & 1) == 0) {
          p_Var3 = p_Var6 + 0x21;
        }
        else {
          p_Var3 = *(__tree_node_base **)(p_Var6 + 0x30);
        }
        goto LAB_000022e9;
      }
LAB_00002303:
      if (local_38 <= uVar5) {
LAB_000023a7:
        *param_1 = (__tree_end_node *)p_Var6;
        return (__tree_node_base **)local_50;
      }
LAB_0000230d:
      local_50 = p_Var6 + 8;
      p_Var3 = *(__tree_node_base **)(p_Var6 + 8);
    } while (*(__tree_node_base **)(p_Var6 + 8) != (__tree_node_base *)0x0);
    *param_1 = (__tree_end_node *)p_Var6;
  }
  return (__tree_node_base **)(__tree_end_node *)local_50;
}



// std::pair<std::string const, std::string >::pair(std::pair<std::string const, std::string >
// const&)

void __thiscall std::pair<>::pair(pair<> *this,pair *param_1)

{
  std::string::string((string *)this,(string *)param_1);
  std::string::string((string *)(this + 0x18),(string *)(param_1 + 0x18));
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// void 
// std::__tree_balance_after_insert<std::__tree_node_base<void*>*>(std::__tree_node_base<void*>*,
// std::__tree_node_base<void*>*)

void std::__tree_balance_after_insert<>(__tree_node_base *param_1,__tree_node_base *param_2)

{
  long lVar1;
  undefined8 *puVar2;
  __tree_node_base *p_Var3;
  long *plVar4;
  undefined8 *puVar5;
  long *plVar6;
  __tree_node_base *p_Var7;
  __tree_node_base _Var8;
  
  _Var8 = (__tree_node_base)(param_2 == param_1);
  param_2[0x18] = _Var8;
  do {
    if ((bool)_Var8) {
      return;
    }
    plVar6 = *(long **)(param_2 + 0x10);
    if ((char)plVar6[3] != '\0') {
      return;
    }
    p_Var7 = (__tree_node_base *)(plVar6 + 3);
    p_Var3 = (__tree_node_base *)plVar6[2];
    plVar4 = *(long **)p_Var3;
    if (plVar4 == plVar6) {
      plVar4 = *(long **)(p_Var3 + 8);
      if ((plVar4 == (long *)0x0) || ((char)plVar4[3] != '\0')) {
        if ((__tree_node_base *)*plVar6 != param_2) {
          plVar4 = (long *)plVar6[1];
          lVar1 = *plVar4;
          plVar6[1] = lVar1;
          if (lVar1 != 0) {
            *(long **)(lVar1 + 0x10) = plVar6;
            p_Var3 = (__tree_node_base *)plVar6[2];
          }
          plVar4[2] = (long)p_Var3;
          puVar2 = (undefined8 *)plVar6[2];
          puVar5 = puVar2 + 1;
          if ((long *)*puVar2 == plVar6) {
            puVar5 = puVar2;
          }
          *puVar5 = plVar4;
          *plVar4 = (long)plVar6;
          plVar6[2] = (long)plVar4;
          p_Var3 = (__tree_node_base *)plVar4[2];
          p_Var7 = (__tree_node_base *)(plVar4 + 3);
        }
        *p_Var7 = (__tree_node_base)0x1;
        p_Var3[0x18] = (__tree_node_base)0x0;
        plVar6 = *(long **)p_Var3;
        lVar1 = plVar6[1];
        *(long *)p_Var3 = lVar1;
        if (lVar1 != 0) {
          *(__tree_node_base **)(lVar1 + 0x10) = p_Var3;
        }
        plVar6[2] = *(long *)(p_Var3 + 0x10);
        puVar2 = *(undefined8 **)(p_Var3 + 0x10);
        puVar5 = puVar2 + 1;
        if ((__tree_node_base *)*puVar2 == p_Var3) {
          puVar5 = puVar2;
        }
        *puVar5 = plVar6;
        plVar6[1] = (long)p_Var3;
LAB_00002583:
        *(long **)(p_Var3 + 0x10) = plVar6;
        return;
      }
    }
    else if ((plVar4 == (long *)0x0) || ((char)plVar4[3] != '\0')) {
      if ((__tree_node_base *)*plVar6 == param_2) {
        lVar1 = *(long *)(param_2 + 8);
        *plVar6 = lVar1;
        if (lVar1 != 0) {
          *(long **)(lVar1 + 0x10) = plVar6;
          p_Var3 = (__tree_node_base *)plVar6[2];
        }
        *(__tree_node_base **)(param_2 + 0x10) = p_Var3;
        puVar2 = (undefined8 *)plVar6[2];
        puVar5 = puVar2 + 1;
        if ((long *)*puVar2 == plVar6) {
          puVar5 = puVar2;
        }
        *puVar5 = param_2;
        *(long **)(param_2 + 8) = plVar6;
        plVar6[2] = (long)param_2;
        p_Var3 = *(__tree_node_base **)(param_2 + 0x10);
        p_Var7 = param_2 + 0x18;
      }
      *p_Var7 = (__tree_node_base)0x1;
      p_Var3[0x18] = (__tree_node_base)0x0;
      plVar6 = *(long **)(p_Var3 + 8);
      lVar1 = *plVar6;
      *(long *)(p_Var3 + 8) = lVar1;
      if (lVar1 != 0) {
        *(__tree_node_base **)(lVar1 + 0x10) = p_Var3;
      }
      plVar6[2] = *(long *)(p_Var3 + 0x10);
      puVar2 = *(undefined8 **)(p_Var3 + 0x10);
      puVar5 = puVar2 + 1;
      if ((__tree_node_base *)*puVar2 == p_Var3) {
        puVar5 = puVar2;
      }
      *puVar5 = plVar6;
      *plVar6 = (long)p_Var3;
      goto LAB_00002583;
    }
    *p_Var7 = (__tree_node_base)0x1;
    _Var8 = (__tree_node_base)(p_Var3 == param_1);
    p_Var3[0x18] = _Var8;
    *(undefined1 *)(plVar4 + 3) = 1;
    param_2 = p_Var3;
  } while( true );
}



// std::__tree_const_iterator<std::__value_type<std::string, std::string >,
// std::__tree_node<std::__value_type<std::string, std::string >, void*>*, long>
// std::__tree<std::__value_type<std::string, std::string >, std::__map_value_compare<std::string,
// std::__value_type<std::string, std::string >, std::less<std::string >, true>,
// std::allocator<std::__value_type<std::string, std::string > > >::find<std::string >(std::string
// const&) const

__tree_end_node * __thiscall std::__tree<>::find<>(__tree<> *this,string *param_1)

{
  __tree_end_node *p_Var1;
  __tree_end_node _Var2;
  string sVar3;
  int iVar4;
  __tree_end_node *p_Var5;
  ulong uVar6;
  __tree_end_node *p_Var7;
  ulong uVar8;
  ulong uVar9;
  string *psVar10;
  
  p_Var1 = (__tree_end_node *)(this + 8);
  p_Var5 = (__tree_end_node *)__lower_bound<>(this,param_1,*(__tree_node **)(this + 8),p_Var1);
  if (p_Var5 != p_Var1) {
    _Var2 = p_Var5[0x20];
    if (((byte)_Var2 & 1) == 0) {
      uVar8 = (ulong)((byte)_Var2 >> 1);
    }
    else {
      uVar8 = *(ulong *)(p_Var5 + 0x28);
    }
    sVar3 = *param_1;
    if (((byte)sVar3 & 1) == 0) {
      uVar9 = (ulong)((byte)sVar3 >> 1);
    }
    else {
      uVar9 = *(ulong *)(param_1 + 8);
    }
    uVar6 = uVar9;
    if (uVar8 < uVar9) {
      uVar6 = uVar8;
    }
    if (uVar6 != 0) {
      if (((byte)sVar3 & 1) == 0) {
        psVar10 = param_1 + 1;
      }
      else {
        psVar10 = *(string **)(param_1 + 0x10);
      }
      if (((byte)_Var2 & 1) == 0) {
        p_Var7 = p_Var5 + 0x21;
      }
      else {
        p_Var7 = *(__tree_end_node **)(p_Var5 + 0x30);
      }
      iVar4 = _memcmp(psVar10,p_Var7,uVar6);
      if (iVar4 != 0) {
        if (iVar4 < 0) {
          return p_Var1;
        }
        return p_Var5;
      }
    }
    if (uVar8 <= uVar9) {
      return p_Var5;
    }
  }
  return p_Var1;
}



// std::__tree_const_iterator<std::__value_type<std::string, std::string >,
// std::__tree_node<std::__value_type<std::string, std::string >, void*>*, long>
// std::__tree<std::__value_type<std::string, std::string >, std::__map_value_compare<std::string,
// std::__value_type<std::string, std::string >, std::less<std::string >, true>,
// std::allocator<std::__value_type<std::string, std::string > > >::__lower_bound<std::string
// >(std::string const&, std::__tree_node<std::__value_type<std::string, std::string >, void*>*,
// std::__tree_end_node<std::__tree_node_base<void*>*>*) const

__tree_end_node * __thiscall
std::__tree<>::__lower_bound<>
          (__tree<> *this,string *param_1,__tree_node *param_2,__tree_end_node *param_3)

{
  __tree_end_node _Var1;
  uint uVar2;
  ulong uVar3;
  __tree_end_node *p_Var4;
  ulong uVar5;
  ulong uVar6;
  string *psVar7;
  
  if (param_2 != (__tree_node *)0x0) {
    if (((byte)*param_1 & 1) == 0) {
      psVar7 = param_1 + 1;
      uVar6 = (ulong)((byte)*param_1 >> 1);
    }
    else {
      uVar6 = *(ulong *)(param_1 + 8);
      psVar7 = *(string **)(param_1 + 0x10);
    }
    do {
      _Var1 = *(__tree_end_node *)(param_2 + 0x20);
      if (((byte)_Var1 & 1) == 0) {
        uVar5 = (ulong)((byte)_Var1 >> 1);
      }
      else {
        uVar5 = *(ulong *)(param_2 + 0x28);
      }
      uVar3 = uVar5;
      if (uVar6 < uVar5) {
        uVar3 = uVar6;
      }
      if (uVar3 == 0) {
LAB_000026bd:
        uVar2 = (uint)(uVar6 < uVar5);
        if (uVar5 < uVar6) {
          uVar2 = 0xffffffff;
        }
      }
      else {
        if (((byte)_Var1 & 1) == 0) {
          p_Var4 = (__tree_end_node *)(param_2 + 0x21);
        }
        else {
          p_Var4 = *(__tree_end_node **)(param_2 + 0x30);
        }
        uVar2 = _memcmp(p_Var4,psVar7,uVar3);
        if (uVar2 == 0) goto LAB_000026bd;
      }
      p_Var4 = (__tree_end_node *)(param_2 + 8);
      if (-1 < (int)uVar2) {
        param_3 = (__tree_end_node *)param_2;
        p_Var4 = (__tree_end_node *)param_2;
      }
      param_2 = *(__tree_node **)p_Var4;
    } while (param_2 != (__tree_node *)0x0);
  }
  return param_3;
}



// WARNING: Globals starting with '_' overlap smaller symbols at the same address

void __GLOBAL__sub_I_errors_cc(void)

{
  undefined1 *puVar1;
  long lVar2;
  byte abStack_338 [16];
  undefined8 uStack_328;
  byte abStack_320 [16];
  undefined *puStack_310;
  undefined1 local_308;
  undefined8 local_307;
  undefined6 uStack_2ff;
  undefined2 local_2f9;
  undefined6 uStack_2f7;
  undefined2 local_2f1;
  undefined8 local_2ef;
  undefined6 uStack_2e7;
  undefined2 local_2e1;
  undefined6 uStack_2df;
  undefined1 local_2d9;
  undefined8 local_2d8;
  undefined8 uStack_2d0;
  undefined8 *local_2c8;
  undefined8 local_2c0;
  undefined8 uStack_2b8;
  undefined8 *local_2b0;
  undefined1 local_2a8;
  undefined8 local_2a7;
  undefined6 uStack_29f;
  undefined2 local_299;
  undefined6 uStack_297;
  undefined2 local_291;
  undefined8 local_28f;
  undefined6 uStack_287;
  undefined2 local_281;
  undefined6 uStack_27f;
  undefined1 local_279;
  undefined1 local_278;
  undefined7 uStack_277;
  undefined1 local_270;
  undefined7 uStack_26f;
  undefined1 local_268;
  undefined2 uStack_267;
  undefined1 uStack_265;
  undefined4 uStack_264;
  undefined1 local_260;
  undefined7 uStack_25f;
  undefined1 local_258;
  undefined7 uStack_257;
  undefined1 local_250;
  undefined2 uStack_24f;
  undefined1 uStack_24d;
  undefined4 uStack_24c;
  undefined1 local_248;
  undefined8 local_247;
  undefined6 uStack_23f;
  undefined2 local_239;
  undefined6 uStack_237;
  undefined1 local_231;
  undefined1 local_230;
  undefined8 local_22f;
  undefined6 uStack_227;
  undefined2 local_221;
  undefined6 uStack_21f;
  undefined1 local_219;
  undefined8 local_218;
  undefined8 uStack_210;
  undefined8 *local_208;
  undefined8 local_200;
  undefined8 uStack_1f8;
  undefined8 *local_1f0;
  undefined8 local_1e8;
  undefined8 uStack_1e0;
  undefined8 *local_1d8;
  undefined8 local_1d0;
  undefined8 uStack_1c8;
  undefined8 *local_1c0;
  undefined8 local_1b8;
  undefined8 uStack_1b0;
  undefined8 *local_1a8;
  undefined8 local_1a0;
  undefined8 uStack_198;
  undefined8 *local_190;
  undefined1 local_188;
  undefined7 uStack_187;
  undefined1 local_180;
  undefined5 uStack_17f;
  undefined2 uStack_17a;
  undefined1 local_178;
  undefined5 uStack_177;
  undefined1 uStack_172;
  undefined1 uStack_171;
  undefined1 local_170;
  undefined7 uStack_16f;
  undefined1 local_168;
  undefined5 uStack_167;
  undefined2 uStack_162;
  undefined1 local_160;
  undefined5 uStack_15f;
  undefined1 uStack_15a;
  undefined1 uStack_159;
  undefined8 local_158;
  undefined8 uStack_150;
  undefined8 *local_148;
  undefined8 local_140;
  undefined8 uStack_138;
  undefined8 *local_130;
  undefined8 local_128;
  undefined8 uStack_120;
  undefined8 *local_118;
  undefined8 local_110;
  undefined8 uStack_108;
  undefined8 *local_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 *local_e8;
  undefined8 local_e0;
  undefined8 uStack_d8;
  undefined8 *local_d0;
  undefined8 local_c8;
  undefined8 uStack_c0;
  undefined8 *local_b8;
  undefined8 local_b0;
  undefined8 uStack_a8;
  undefined8 *local_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 *local_88;
  undefined8 local_80;
  undefined8 uStack_78;
  undefined8 *local_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 *local_58;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 *local_40;
  long local_38;
  
  local_38 = *(long *)PTR____stack_chk_guard_00005058;
  local_308 = 0x2c;
  uStack_2f7 = 0x4b4341424c4c;
  uStack_2ff = 0x5f44494c4156;
  local_2f9 = 0x4143;
  local_307 = 0x4e495f4e49574c51;
  local_2f1 = 0x2c00;
  uStack_2df = 0x4b4341424c4c;
  uStack_2e7 = 0x5f44494c4156;
  local_2e1 = 0x4143;
  local_2ef = 0x4e495f4e49574c51;
  local_2d9 = 0;
  local_2c8 = (undefined8 *)0x0;
  uStack_2d0 = 0;
  local_2d8 = 0;
  puStack_310 = (undefined *)0x279d;
  local_2c8 = operator_new(0x20);
  local_2d8 = 0x21;
  uStack_2d0 = 0x1a;
  local_2c8[2] = 0x52415f464f5f5245;
  local_2c8[1] = 0x424d554e5f474e4f;
  *local_2c8 = 0x52575f4e49574c51;
  *(undefined2 *)(local_2c8 + 3) = 0x5347;
  *(undefined1 *)((long)local_2c8 + 0x1a) = 0;
  local_2b0 = (undefined8 *)0x0;
  uStack_2b8 = 0;
  local_2c0 = 0;
  puStack_310 = (undefined *)0x2801;
  local_2b0 = operator_new(0x20);
  local_2c0 = 0x21;
  uStack_2b8 = 0x1a;
  local_2b0[2] = 0x52415f464f5f5245;
  local_2b0[1] = 0x424d554e5f474e4f;
  *local_2b0 = 0x52575f4e49574c51;
  *(undefined2 *)(local_2b0 + 3) = 0x5347;
  *(undefined1 *)((long)local_2b0 + 0x1a) = 0;
  local_2a8 = 0x2c;
  uStack_297 = 0x455059545f47;
  uStack_29f = 0x5f44494c4156;
  local_299 = 0x5241;
  local_2a7 = 0x4e495f4e49574c51;
  local_291 = 0x2c00;
  uStack_27f = 0x455059545f47;
  uStack_287 = 0x5f44494c4156;
  local_281 = 0x5241;
  local_28f = 0x4e495f4e49574c51;
  local_279 = 0;
  local_278 = 0x26;
  uStack_265 = 0x52;
  uStack_26f = 0x455f454d49544e;
  local_268 = 0x52;
  uStack_277 = 0x525f4e49574c51;
  local_270 = 0x55;
  uStack_267 = 0x4f52;
  uStack_264 = 0;
  local_260 = 0x26;
  uStack_24d = 0x52;
  uStack_24f = 0x4f52;
  uStack_257 = 0x455f454d49544e;
  local_250 = 0x52;
  uStack_25f = 0x525f4e49574c51;
  local_258 = 0x55;
  uStack_24c = 0;
  local_248 = 0x2c;
  uStack_237 = 0x4e4f49545045;
  uStack_23f = 0x455f574f4e4b;
  local_239 = 0x4358;
  local_247 = 0x4e555f4e49574c51;
  local_231 = 0;
  local_230 = 0x2c;
  uStack_21f = 0x4e4f49545045;
  uStack_227 = 0x455f574f4e4b;
  local_221 = 0x4358;
  local_22f = 0x4e555f4e49574c51;
  local_219 = 0;
  local_208 = (undefined8 *)0x0;
  uStack_210 = 0;
  local_218 = 0;
  puStack_310 = (undefined *)0x29a8;
  local_208 = operator_new(0x20);
  local_218 = 0x21;
  uStack_210 = 0x18;
  local_208[2] = 0x4b4341424c4c4143;
  local_208[1] = 0x5f44494c41564e49;
  *local_208 = 0x5f4e49574e45504f;
  *(undefined1 *)(local_208 + 3) = 0;
  local_1f0 = (undefined8 *)0x0;
  uStack_1f8 = 0;
  local_200 = 0;
  puStack_310 = (undefined *)0x2a06;
  local_1f0 = operator_new(0x20);
  local_200 = 0x21;
  uStack_1f8 = 0x18;
  local_1f0[2] = 0x4b4341424c4c4143;
  local_1f0[1] = 0x5f44494c41564e49;
  *local_1f0 = 0x5f4e49574e45504f;
  *(undefined1 *)(local_1f0 + 3) = 0;
  local_1d8 = (undefined8 *)0x0;
  uStack_1e0 = 0;
  local_1e8 = 0;
  puStack_310 = (undefined *)0x2a51;
  local_1d8 = operator_new(0x20);
  local_1e8 = 0x21;
  uStack_1e0 = 0x1c;
  local_1d8[2] = 0x5f464f5f5245424d;
  local_1d8[1] = 0x554e5f474e4f5257;
  *local_1d8 = 0x5f4e49574e45504f;
  *(undefined4 *)(local_1d8 + 3) = 0x53475241;
  *(undefined1 *)((long)local_1d8 + 0x1c) = 0;
  local_1c0 = (undefined8 *)0x0;
  uStack_1c8 = 0;
  local_1d0 = 0;
  puStack_310 = (undefined *)0x2aac;
  local_1c0 = operator_new(0x20);
  local_1d0 = 0x21;
  uStack_1c8 = 0x1c;
  local_1c0[2] = 0x5f464f5f5245424d;
  local_1c0[1] = 0x554e5f474e4f5257;
  *local_1c0 = 0x5f4e49574e45504f;
  *(undefined4 *)(local_1c0 + 3) = 0x53475241;
  *(undefined1 *)((long)local_1c0 + 0x1c) = 0;
  local_1a8 = (undefined8 *)0x0;
  uStack_1b0 = 0;
  local_1b8 = 0;
  puStack_310 = (undefined *)0x2afe;
  local_1a8 = operator_new(0x20);
  local_1b8 = 0x21;
  uStack_1b0 = 0x18;
  local_1a8[2] = 0x455059545f475241;
  local_1a8[1] = 0x5f44494c41564e49;
  *local_1a8 = 0x5f4e49574e45504f;
  *(undefined1 *)(local_1a8 + 3) = 0;
  local_190 = (undefined8 *)0x0;
  uStack_198 = 0;
  local_1a0 = 0;
  puStack_310 = (undefined *)0x2b52;
  local_190 = operator_new(0x20);
  local_1a0 = 0x21;
  uStack_198 = 0x18;
  local_190[2] = 0x455059545f475241;
  local_190[1] = 0x5f44494c41564e49;
  *local_190 = 0x5f4e49574e45504f;
  *(undefined1 *)(local_190 + 3) = 0;
  uStack_171 = 0;
  local_188 = 0x2a;
  uStack_177 = 0x524f525245;
  uStack_17f = 0x49544e5552;
  uStack_17a = 0x454d;
  local_178 = 0x5f;
  uStack_187 = 0x4e49574e45504f;
  local_180 = 0x5f;
  uStack_172 = 0;
  uStack_159 = 0;
  local_170 = 0x2a;
  uStack_15f = 0x524f525245;
  uStack_167 = 0x49544e5552;
  uStack_162 = 0x454d;
  local_160 = 0x5f;
  uStack_16f = 0x4e49574e45504f;
  local_168 = 0x5f;
  uStack_15a = 0;
  local_148 = (undefined8 *)0x0;
  uStack_150 = 0;
  local_158 = 0;
  puStack_310 = (undefined *)0x2c1e;
  local_148 = operator_new(0x20);
  local_158 = 0x21;
  uStack_150 = 0x1a;
  local_148[2] = 0x495450454358455f;
  local_148[1] = 0x574f4e4b4e555f45;
  *local_148 = 0x474153554b534944;
  *(undefined2 *)(local_148 + 3) = 0x4e4f;
  *(undefined1 *)((long)local_148 + 0x1a) = 0;
  local_130 = (undefined8 *)0x0;
  uStack_138 = 0;
  local_140 = 0;
  puStack_310 = (undefined *)0x2c82;
  local_130 = operator_new(0x20);
  local_140 = 0x21;
  uStack_138 = 0x1a;
  local_130[2] = 0x495450454358455f;
  local_130[1] = 0x574f4e4b4e555f45;
  *local_130 = 0x474153554b534944;
  *(undefined2 *)(local_130 + 3) = 0x4e4f;
  *(undefined1 *)((long)local_130 + 0x1a) = 0;
  local_118 = (undefined8 *)0x0;
  uStack_120 = 0;
  local_128 = 0;
  puStack_310 = (undefined *)0x2cd3;
  local_118 = operator_new(0x20);
  local_128 = 0x21;
  uStack_120 = 0x1a;
  local_118[2] = 0x41424c4c41435f44;
  local_118[1] = 0x494c41564e495f45;
  *local_118 = 0x474153554b534944;
  *(undefined2 *)(local_118 + 3) = 0x4b43;
  *(undefined1 *)((long)local_118 + 0x1a) = 0;
  local_100 = (undefined8 *)0x0;
  uStack_108 = 0;
  local_110 = 0;
  puStack_310 = (undefined *)0x2d2d;
  local_100 = operator_new(0x20);
  local_110 = 0x21;
  uStack_108 = 0x1a;
  local_100[2] = 0x41424c4c41435f44;
  local_100[1] = 0x494c41564e495f45;
  *local_100 = 0x474153554b534944;
  *(undefined2 *)(local_100 + 3) = 0x4b43;
  *(undefined1 *)((long)local_100 + 0x1a) = 0;
  local_e8 = (undefined8 *)0x0;
  uStack_f0 = 0;
  local_f8 = 0;
  puStack_310 = (undefined *)0x2d7e;
  local_e8 = operator_new(0x20);
  local_f8 = 0x21;
  uStack_f0 = 0x1e;
  *(undefined8 *)((long)local_e8 + 0x16) = 0x534752415f464f5f;
  local_e8[2] = 0x4f5f5245424d554e;
  local_e8[1] = 0x5f474e4f52575f45;
  *local_e8 = 0x474153554b534944;
  *(undefined1 *)((long)local_e8 + 0x1e) = 0;
  local_d0 = (undefined8 *)0x0;
  uStack_d8 = 0;
  local_e0 = 0;
  puStack_310 = (undefined *)0x2de3;
  local_d0 = operator_new(0x20);
  local_e0 = 0x21;
  uStack_d8 = 0x1e;
  *(undefined8 *)((long)local_d0 + 0x16) = 0x534752415f464f5f;
  local_d0[2] = 0x4f5f5245424d554e;
  local_d0[1] = 0x5f474e4f52575f45;
  *local_d0 = 0x474153554b534944;
  *(undefined1 *)((long)local_d0 + 0x1e) = 0;
  local_b8 = (undefined8 *)0x0;
  uStack_c0 = 0;
  local_c8 = 0;
  puStack_310 = (undefined *)0x2e3c;
  local_b8 = operator_new(0x20);
  local_c8 = 0x21;
  uStack_c0 = 0x1a;
  local_b8[2] = 0x59545f4752415f44;
  local_b8[1] = 0x494c41564e495f45;
  *local_b8 = 0x474153554b534944;
  *(undefined2 *)(local_b8 + 3) = 0x4550;
  *(undefined1 *)((long)local_b8 + 0x1a) = 0;
  local_a0 = (undefined8 *)0x0;
  uStack_a8 = 0;
  local_b0 = 0;
  puStack_310 = (undefined *)0x2e96;
  local_a0 = operator_new(0x20);
  local_b0 = 0x21;
  uStack_a8 = 0x1a;
  local_a0[2] = 0x59545f4752415f44;
  local_a0[1] = 0x494c41564e495f45;
  *local_a0 = 0x474153554b534944;
  *(undefined2 *)(local_a0 + 3) = 0x4550;
  *(undefined1 *)((long)local_a0 + 0x1a) = 0;
  local_88 = (undefined8 *)0x0;
  uStack_90 = 0;
  local_98 = 0;
  puStack_310 = (undefined *)0x2ee4;
  local_88 = operator_new(0x20);
  local_98 = 0x21;
  uStack_90 = 0x17;
  *(undefined8 *)((long)local_88 + 0xf) = 0x524f5252455f454d;
  local_88[1] = 0x4d49544e55525f45;
  *local_88 = 0x474153554b534944;
  *(undefined1 *)((long)local_88 + 0x17) = 0;
  local_70 = (undefined8 *)0x0;
  uStack_78 = 0;
  local_80 = 0;
  puStack_310 = (undefined *)0x2f38;
  local_70 = operator_new(0x20);
  local_80 = 0x21;
  uStack_78 = 0x17;
  *(undefined8 *)((long)local_70 + 0xf) = 0x524f5252455f454d;
  local_70[1] = 0x4d49544e55525f45;
  *local_70 = 0x474153554b534944;
  *(undefined1 *)((long)local_70 + 0x17) = 0;
  local_58 = (undefined8 *)0x0;
  uStack_60 = 0;
  local_68 = 0;
  puStack_310 = (undefined *)0x2f74;
  local_58 = operator_new(0x20);
  local_68 = 0x21;
  uStack_60 = 0x1a;
  local_58[2] = 0x495450454358455f;
  local_58[1] = 0x574f4e4b4e555f45;
  *local_58 = 0x474153554b534944;
  *(undefined2 *)(local_58 + 3) = 0x4e4f;
  *(undefined1 *)((long)local_58 + 0x1a) = 0;
  local_40 = (undefined8 *)0x0;
  uStack_48 = 0;
  local_50 = 0;
  puStack_310 = (undefined *)0x2fcb;
  local_40 = operator_new(0x20);
  puVar1 = &local_308;
  local_50 = 0x21;
  uStack_48 = 0x1a;
  local_40[2] = 0x495450454358455f;
  local_40[1] = 0x574f4e4b4e555f45;
  *local_40 = 0x474153554b534944;
  *(undefined2 *)(local_40 + 3) = 0x4e4f;
  *(undefined1 *)((long)local_40 + 0x1a) = 0;
  _DAT_000052e0 = 0;
  _DAT_000052d8 = 0;
  _Errors = &DAT_000052d8;
  lVar2 = 0x2d0;
  do {
    puStack_310 = (undefined *)0x3038;
    std::__tree<>::__emplace_hint_unique_key_args<>(&_Errors,&DAT_000052d8,puVar1,puVar1);
    puVar1 = puVar1 + 0x30;
    lVar2 = lVar2 + -0x30;
  } while (lVar2 != 0);
  lVar2 = 0x2d0;
  do {
    if ((abStack_320[lVar2] & 1) != 0) {
      puStack_310 = (undefined *)0x305e;
      operator_delete(*(void **)((long)&puStack_310 + lVar2));
    }
    if ((abStack_338[lVar2] & 1) != 0) {
      puStack_310 = (undefined *)0x3075;
      operator_delete(*(void **)((long)&uStack_328 + lVar2));
    }
    lVar2 = lVar2 + -0x30;
  } while (lVar2 != 0);
  puStack_310 = (undefined *)0x3095;
  ___cxa_atexit(PTR__map_00005028,&_Errors,0);
  if (*(long *)PTR____stack_chk_guard_00005058 == local_38) {
    return;
  }
                    // WARNING: Subroutine does not return
  puStack_310 = &UNK_000030bc;
  ___stack_chk_fail();
}



// FetchDiskUsage(char const*)

char * FetchDiskUsage(char *param_1)

{
  int iVar1;
  runtime_error *this;
  char *in_RSI;
  statvfs local_58;
  
  local_58.f_flag = 0;
  local_58.f_namemax = 0;
  local_58.f_ffree = 0;
  local_58.f_favail = 0;
  local_58.f_fsid = 0;
  local_58.f_blocks = 0;
  local_58.f_bfree = 0;
  local_58.f_bavail = 0;
  local_58.f_files = 0;
  local_58.f_bsize = 0;
  local_58.f_frsize = 0;
  iVar1 = _statvfs(in_RSI,&local_58);
  if (iVar1 == 0) {
    *(ulong *)param_1 = (local_58._24_8_ & 0xffffffff) * local_58.f_frsize;
    *(ulong *)(param_1 + 8) = ((ulong)local_58._16_8_ >> 0x20) * local_58.f_frsize;
    *(ulong *)(param_1 + 0x10) = (local_58._16_8_ & 0xffffffff) * local_58.f_frsize;
    return param_1;
  }
  this = (runtime_error *)___cxa_allocate_exception(0x10);
  std::runtime_error::runtime_error(this,"Get diskusage failed");
                    // WARNING: Subroutine does not return
  ___cxa_throw(this,PTR_typeinfo_00005038,PTR__runtime_error_00005020);
}



// RunFetchDiskUsage(Napi::CallbackInfo const&)

undefined1  [16] RunFetchDiskUsage(CallbackInfo *param_1)

{
  int iVar1;
  napi_env__ *pnVar2;
  napi_env__ *pnVar3;
  undefined1 auVar4 [16];
  char local_b8 [8];
  undefined1 local_b0 [8];
  undefined1 local_a8 [32];
  undefined8 local_88;
  napi_env__ *local_80;
  undefined8 local_78;
  napi_env__ *local_70;
  napi_env__ *local_68;
  void *local_60;
  Error local_58 [24];
  byte local_40;
  void *local_30;
  
  pnVar2 = *(napi_env__ **)(param_1 + 8);
  if (*(long *)(param_1 + 0x20) == 0) {
    Errors::throwError(pnVar2,"DISKUSAGE_WRONG_NUMBER_OF_ARGS","");
    iVar1 = _napi_get_undefined(pnVar2,&local_80);
    local_68 = local_80;
    if (iVar1 == 0) goto LAB_00003364;
    Napi::Error::New(local_58,pnVar2);
    Napi::Error::ThrowAsJavaScriptException(local_58);
  }
  else {
    if (**(long **)(param_1 + 0x28) != 0) {
      iVar1 = _napi_typeof(pnVar2,**(long **)(param_1 + 0x28),&local_80);
      if (iVar1 == 0) {
        if ((int)local_80 == 4) {
          pnVar3 = *(napi_env__ **)(param_1 + 8);
          if (*(long *)(param_1 + 0x20) == 0) {
            iVar1 = _napi_get_undefined(pnVar3,&local_88);
            if (iVar1 != 0) {
              Napi::Error::New(local_58,pnVar3);
              Napi::Error::ThrowAsJavaScriptException(local_58);
              if ((local_40 & 1) != 0) {
                operator_delete(local_30);
              }
              Napi::Reference<>::~Reference((Reference<> *)local_58);
              local_88 = 0;
              pnVar3 = (napi_env__ *)0x0;
            }
          }
          else {
            local_88 = **(undefined8 **)(param_1 + 0x28);
          }
          local_80 = pnVar3;
          local_78 = local_88;
          Napi::String::Utf8Value();
          FetchDiskUsage(local_b8);
          if (((ulong)local_70 & 1) != 0) {
            operator_delete(local_60);
          }
          iVar1 = _napi_create_object(pnVar2,&local_80);
          local_68 = local_80;
          local_70 = pnVar2;
          if (iVar1 != 0) {
            Napi::Error::New(local_58,pnVar2);
            Napi::Error::ThrowAsJavaScriptException(local_58);
            if ((local_40 & 1) != 0) {
              operator_delete(local_30);
            }
            Napi::Reference<>::~Reference((Reference<> *)local_58);
            local_68 = (napi_env__ *)0x0;
            local_70 = (napi_env__ *)0x0;
          }
          auVar4 = Napi::String::New(pnVar2,"available");
          Napi::Object::Set<>(&local_70,auVar4._0_8_,auVar4._8_8_,local_b8);
          auVar4 = Napi::String::New(pnVar2,"free");
          Napi::Object::Set<>(&local_70,auVar4._0_8_,auVar4._8_8_,local_b0);
          auVar4 = Napi::String::New(pnVar2,"total");
          Napi::Object::Set<>(&local_70,auVar4._0_8_,auVar4._8_8_,local_a8);
          pnVar2 = local_70;
          goto LAB_00003364;
        }
      }
      else {
        Napi::Error::New(local_58,pnVar2);
        Napi::Error::ThrowAsJavaScriptException(local_58);
        if ((local_40 & 1) != 0) {
          operator_delete(local_30);
        }
        Napi::Reference<>::~Reference((Reference<> *)local_58);
      }
    }
    Errors::throwError(pnVar2,"DISKUSAGE_WRONG_NUMBER_OF_ARGS",
                       ":  The \"path\" argument must be one of type string");
    iVar1 = _napi_get_undefined(pnVar2,&local_80);
    local_68 = local_80;
    if (iVar1 == 0) goto LAB_00003364;
    Napi::Error::New(local_58,pnVar2);
    Napi::Error::ThrowAsJavaScriptException(local_58);
  }
  if ((local_40 & 1) != 0) {
    operator_delete(local_30);
  }
  Napi::Reference<>::~Reference((Reference<> *)local_58);
  local_68 = (napi_env__ *)0x0;
  pnVar2 = (napi_env__ *)0x0;
LAB_00003364:
  auVar4._8_8_ = local_68;
  auVar4._0_8_ = pnVar2;
  return auVar4;
}



// Napi::String::Utf8Value() const

void Napi::String::Utf8Value(void)

{
  int iVar1;
  long lVar2;
  void *pvVar3;
  undefined8 *in_RSI;
  ulong *in_RDI;
  Error local_70 [24];
  byte local_58;
  void *local_48;
  char local_40 [8];
  undefined8 local_38;
  ulong uStack_30;
  void *local_28;
  
  iVar1 = _napi_get_value_string_utf8(*in_RSI,in_RSI[1],0,0,local_40);
  if (iVar1 == 0) {
    local_38 = 0;
    uStack_30 = 0;
    local_28 = (void *)0x0;
    std::string::reserve((ulong)&local_38);
    std::string::resize((ulong)&local_38,local_40[0]);
    if ((local_38 & 1) == 0) {
      lVar2 = 0x16;
      pvVar3 = (void *)((long)&local_38 + 1);
    }
    else {
      lVar2 = (local_38 & 0xfffffffffffffffe) - 1;
      pvVar3 = local_28;
    }
    iVar1 = _napi_get_value_string_utf8(*in_RSI,in_RSI[1],pvVar3,lVar2,0);
    if (iVar1 == 0) {
      in_RDI[2] = (ulong)local_28;
      in_RDI[1] = uStack_30;
      *in_RDI = local_38;
    }
    else {
      Error::New(local_70,(napi_env__ *)*in_RSI);
      Error::ThrowAsJavaScriptException(local_70);
      if ((local_58 & 1) != 0) {
        operator_delete(local_48);
      }
      Reference<>::~Reference((Reference<> *)local_70);
      in_RDI[2] = 0;
      in_RDI[1] = 0;
      *in_RDI = 0;
      if ((local_38 & 1) != 0) {
        operator_delete(local_28);
      }
    }
  }
  else {
    Error::New(local_70,(napi_env__ *)*in_RSI);
    Error::ThrowAsJavaScriptException(local_70);
    if ((local_58 & 1) != 0) {
      operator_delete(local_48);
    }
    Reference<>::~Reference((Reference<> *)local_70);
    in_RDI[2] = 0;
    in_RDI[1] = 0;
    *in_RDI = 0;
  }
  return;
}



// void Napi::Object::Set<unsigned long long>(Napi::Value, unsigned long long const&)

void Napi::Object::Set<>(undefined8 *param_1,undefined8 param_2,undefined8 param_3,
                        undefined8 *param_4)

{
  napi_env__ *pnVar1;
  undefined8 uVar2;
  int iVar3;
  undefined8 local_68;
  Error local_60 [24];
  byte local_48;
  void *local_38;
  
  pnVar1 = (napi_env__ *)*param_1;
  uVar2 = param_1[1];
  iVar3 = _napi_create_double(SUB84(((double)CONCAT44(0x43300000,(int)*param_4) - 4503599627370496.0
                                    ) + ((double)CONCAT44(0x45300000,(int)((ulong)*param_4 >> 0x20))
                                        - 1.9342813113834067e+25),0),pnVar1,&local_68);
  if (iVar3 != 0) {
    Error::New(local_60,pnVar1);
    Error::ThrowAsJavaScriptException(local_60);
    if ((local_48 & 1) != 0) {
      operator_delete(local_38);
    }
    Reference<>::~Reference((Reference<> *)local_60);
    local_68 = 0;
  }
  iVar3 = _napi_set_property(pnVar1,uVar2,param_3,local_68);
  if (iVar3 != 0) {
    Error::New(local_60,(napi_env__ *)*param_1);
    Error::ThrowAsJavaScriptException(local_60);
    if ((local_48 & 1) != 0) {
      operator_delete(local_38);
    }
    Reference<>::~Reference((Reference<> *)local_60);
  }
  return;
}



// Napi::String::New(napi_env__*, char const*)

undefined1  [16] Napi::String::New(napi_env__ *param_1,char *param_2)

{
  int iVar1;
  size_t sVar2;
  undefined1 auVar3 [16];
  Error local_50 [24];
  byte local_38;
  void *local_28;
  undefined8 local_20;
  
  sVar2 = _strlen(param_2);
  iVar1 = _napi_create_string_utf8(param_1,param_2,sVar2,&local_20);
  if (iVar1 != 0) {
    Error::New(local_50,param_1);
    Error::ThrowAsJavaScriptException(local_50);
    if ((local_38 & 1) != 0) {
      operator_delete(local_28);
    }
    Reference<>::~Reference((Reference<> *)local_50);
    local_20 = 0;
    param_1 = (napi_env__ *)0x0;
  }
  auVar3._8_8_ = local_20;
  auVar3._0_8_ = param_1;
  return auVar3;
}



// GetDiskUsage::Init(Napi::Env, Napi::Object)

undefined1  [16] __thiscall
GetDiskUsage::Init(GetDiskUsage *this,napi_env__ *param_2,undefined8 param_3)

{
  int iVar1;
  undefined8 extraout_RDX;
  undefined8 extraout_RDX_00;
  undefined1 auVar2 [16];
  Error local_58 [24];
  byte local_40;
  void *local_30;
  
  Napi::String::New((napi_env__ *)this,"getDiskUsage");
  Napi::Function::New<>((napi_env__ *)this,RunFetchDiskUsage,(char *)0x0,(void *)0x0);
  iVar1 = _napi_set_property(param_2,param_3,extraout_RDX,extraout_RDX_00);
  if (iVar1 != 0) {
    Napi::Error::New(local_58,param_2);
    Napi::Error::ThrowAsJavaScriptException(local_58);
    if ((local_40 & 1) != 0) {
      operator_delete(local_30);
    }
    Napi::Reference<>::~Reference((Reference<> *)local_58);
  }
  auVar2._8_8_ = param_3;
  auVar2._0_8_ = param_2;
  return auVar2;
}



// Napi::Function Napi::Function::New<Napi::Value (*)(Napi::CallbackInfo const&)>(napi_env__*,
// Napi::Value (*)(Napi::CallbackInfo const&), char const*, void*)

undefined1  [16]
Napi::Function::New<>
          (napi_env__ *param_1,_func_Value_CallbackInfo_ptr *param_2,char *param_3,void *param_4)

{
  undefined8 uVar1;
  int iVar2;
  undefined8 *puVar3;
  undefined1 auVar4 [16];
  undefined8 local_88;
  undefined8 local_80;
  undefined8 local_78;
  ulong local_70;
  undefined8 local_68;
  void *local_60;
  undefined4 local_58;
  undefined8 local_50;
  undefined8 local_48;
  void *local_40;
  undefined8 local_38;
  
  puVar3 = operator_new(0x10);
  *puVar3 = param_2;
  puVar3[1] = param_4;
  iVar2 = _napi_create_function
                    (param_1,param_3,0xffffffffffffffff,PTR_Wrapper_00005010,puVar3,&local_38);
  uVar1 = local_38;
  if (((iVar2 == 0) && (iVar2 = _napi_create_symbol(param_1,0,&local_48), iVar2 == 0)) &&
     (iVar2 = _napi_create_external
                        (param_1,puVar3,
                         details::
                         AttachData<>(napi_env__*,napi_value__*,Napi::details::CallbackData<>*,void(*)(napi_env__*,void*,void*),void*)
                         ::{lambda(napi_env__*,void*,void*)#1}::__invoke,0,&local_40), iVar2 == 0))
  {
    local_88 = 0;
    local_80 = local_48;
    local_68 = 0;
    local_70 = 0;
    local_78 = 0;
    local_60 = local_40;
    local_58 = 0;
    local_50 = 0;
    iVar2 = _napi_define_properties(param_1,uVar1,1);
    if (iVar2 == 0) goto LAB_00003b25;
  }
  operator_delete(puVar3);
  Error::New((Error *)&local_88,param_1);
  Error::ThrowAsJavaScriptException((Error *)&local_88);
  if ((local_70 & 1) != 0) {
    operator_delete(local_60);
  }
  Reference<>::~Reference((Reference<> *)&local_88);
  local_38 = 0;
  param_1 = (napi_env__ *)0x0;
LAB_00003b25:
  auVar4._8_8_ = local_38;
  auVar4._0_8_ = param_1;
  return auVar4;
}



// Napi::Error::ThrowAsJavaScriptException() const

void __thiscall Napi::Error::ThrowAsJavaScriptException(Error *this)

{
  undefined8 uVar1;
  code *pcVar2;
  int iVar3;
  undefined8 extraout_RDX;
  Error local_58 [24];
  byte local_40;
  void *local_30;
  napi_env__ *local_28;
  undefined8 local_20;
  
  local_28 = *(napi_env__ **)this;
  iVar3 = _napi_open_handle_scope();
  if (iVar3 != 0) {
    New(local_58,local_28);
    ThrowAsJavaScriptException(local_58);
    if ((local_40 & 1) != 0) {
      operator_delete(local_30);
    }
    Reference<>::~Reference((Reference<> *)local_58);
  }
  if (*(long *)(this + 8) != 0) {
    uVar1 = *(undefined8 *)this;
    Reference<>::Value((Reference<> *)this);
    iVar3 = _napi_throw(uVar1,extraout_RDX);
    if (iVar3 != 0) {
      Fatal("Error::ThrowAsJavaScriptException","napi_throw");
      goto LAB_00003c7d;
    }
  }
  iVar3 = _napi_close_handle_scope(local_28,local_20);
  if (iVar3 == 0) {
    return;
  }
  Fatal("HandleScope::~HandleScope","napi_close_handle_scope");
LAB_00003c7d:
                    // WARNING: Does not return
  pcVar2 = (code *)invalidInstructionException();
  (*pcVar2)();
}



// Napi::Reference<Napi::Object>::Value() const

undefined1  [16] __thiscall Napi::Reference<>::Value(Reference<> *this)

{
  int iVar1;
  undefined8 uVar2;
  undefined1 auVar3 [16];
  Error local_48 [24];
  byte local_30;
  void *local_20;
  undefined8 local_18;
  
  if (*(long *)(this + 8) == 0) {
    uVar2 = *(undefined8 *)this;
    local_18 = 0;
  }
  else {
    iVar1 = _napi_get_reference_value(*(undefined8 *)this,*(long *)(this + 8),&local_18);
    if (iVar1 == 0) {
      uVar2 = *(undefined8 *)this;
    }
    else {
      Error::New(local_48,*(napi_env__ **)this);
      Error::ThrowAsJavaScriptException(local_48);
      if ((local_30 & 1) != 0) {
        operator_delete(local_20);
      }
      ~Reference((Reference<> *)local_48);
      local_18 = 0;
      uVar2 = 0;
    }
  }
  auVar3._8_8_ = local_18;
  auVar3._0_8_ = uVar2;
  return auVar3;
}



// Napi::details::CallbackData<Napi::Value (*)(Napi::CallbackInfo const&),
// Napi::Value>::Wrapper(napi_env__*, napi_callback_info__*)

undefined8 Napi::details::CallbackData<>::Wrapper(napi_env__ *param_1,napi_callback_info__ *param_2)

{
  code *pcVar1;
  undefined8 extraout_RDX;
  CallbackInfo local_80 [96];
  void *local_20;
  undefined8 *local_18;
  
  CallbackInfo::CallbackInfo(local_80,param_1,param_2);
  pcVar1 = (code *)*local_18;
  local_18 = (undefined8 *)local_18[1];
  (*pcVar1)(local_80);
  if (local_20 != (void *)0x0) {
    operator_delete__(local_20);
  }
  return extraout_RDX;
}



// __invoke(napi_env__*, void*, void*)

void Napi::details::
     AttachData<>(napi_env__*,napi_value__*,Napi::details::CallbackData<>*,void(*)(napi_env__*,void*,void*),void*)
     ::{lambda(napi_env__*,void*,void*)#1}::__invoke
               (napi_env__ *param_1,void *param_2,void *param_3)

{
  if (param_2 != (void *)0x0) {
    operator_delete(param_2);
    return;
  }
  return;
}



// Napi::CallbackInfo::CallbackInfo(napi_env__*, napi_callback_info__*)

void __thiscall
Napi::CallbackInfo::CallbackInfo
          (CallbackInfo *this,napi_env__ *param_1,napi_callback_info__ *param_2)

{
  undefined1 auVar1 [16];
  int iVar2;
  void *pvVar3;
  ulong uVar4;
  Error local_58 [24];
  byte local_40;
  void *local_30;
  
  *(undefined8 *)this = 6;
  *(napi_env__ **)(this + 8) = param_1;
  *(napi_callback_info__ **)(this + 0x10) = param_2;
  *(undefined8 *)(this + 0x18) = 0;
  *(undefined8 *)(this + 0x68) = 0;
  *(undefined8 *)(this + 0x60) = 0;
  *(undefined8 *)(this + 0x20) = 6;
  *(CallbackInfo **)(this + 0x28) = this + 0x30;
  iVar2 = _napi_get_cb_info(param_1,param_2,this + 0x20,this + 0x30,this + 0x18,this + 0x68);
  if (iVar2 == 0) {
    if (*(ulong *)(this + 0x20) <= *(ulong *)this) {
      return;
    }
    auVar1._8_8_ = 0;
    auVar1._0_8_ = *(ulong *)(this + 0x20);
    uVar4 = 0xffffffffffffffff;
    if (SUB168(auVar1 * ZEXT816(8),8) == 0) {
      uVar4 = SUB168(auVar1 * ZEXT816(8),0);
    }
    pvVar3 = operator_new__(uVar4);
    *(void **)(this + 0x60) = pvVar3;
    *(void **)(this + 0x28) = pvVar3;
    iVar2 = _napi_get_cb_info(param_1,param_2,this + 0x20,pvVar3,0,0);
    if (iVar2 == 0) {
      return;
    }
    Error::New(local_58,*(napi_env__ **)(this + 8));
    Error::ThrowAsJavaScriptException(local_58);
  }
  else {
    Error::New(local_58,*(napi_env__ **)(this + 8));
    Error::ThrowAsJavaScriptException(local_58);
  }
  if ((local_40 & 1) != 0) {
    operator_delete(local_30);
  }
  Reference<>::~Reference((Reference<> *)local_58);
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

void __Unwind_Resume(_Unwind_Exception *exception_object)

{
                    // WARNING: Could not recover jumptable at 0x00003ebc. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR___Unwind_Resume_00005068)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// std::__basic_string_common<true>::__throw_length_error() const

void std::__basic_string_common<true>::__throw_length_error(void)

{
                    // WARNING: Could not recover jumptable at 0x00003ec2. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR___throw_length_error_00005070)();
  return;
}



// std::logic_error::logic_error(char const*)

void __thiscall std::logic_error::logic_error(logic_error *this,char *param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003ec8. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_logic_error_00005078)();
  return;
}



// std::runtime_error::runtime_error(char const*)

void __thiscall std::runtime_error::runtime_error(runtime_error *this,char *param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003ece. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_runtime_error_00005080)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// std::string::resize(unsigned long, char)

void std::string::resize(ulong param_1,char param_2)

{
                    // WARNING: Could not recover jumptable at 0x00003ed4. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_resize_00005088)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// std::string::reserve(unsigned long)

void std::string::reserve(ulong param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003eda. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_reserve_00005090)();
  return;
}



// std::string::string(std::string const&)

void __thiscall std::string::string(string *this,string *param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003ee0. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_string_00005098)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// std::terminate()

void std::terminate(void)

{
                    // WARNING: Could not recover jumptable at 0x00003ee6. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_terminate_000050a0)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// operator delete[](void*)

void operator_delete__(void *param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003eec. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_operator_delete___000050a8)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// operator delete(void*)

void operator_delete(void *param_1)

{
                    // WARNING: Could not recover jumptable at 0x00003ef2. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR_operator_delete_000050b0)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// operator new[](unsigned long)

void * operator_new__(ulong param_1)

{
  void *pvVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003ef8. Too many branches
                    // WARNING: Treating indirect jump as call
  pvVar1 = (void *)(*(code *)PTR_operator_new___000050b8)();
  return pvVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked
// operator new(unsigned long)

void * operator_new(ulong param_1)

{
  void *pvVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003efe. Too many branches
                    // WARNING: Treating indirect jump as call
  pvVar1 = (void *)(*(code *)PTR_operator_new_000050c0)();
  return pvVar1;
}



void ___cxa_allocate_exception(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f04. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_allocate_exception_000050c8)();
  return;
}



void ___cxa_atexit(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f0a. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_atexit_000050d0)();
  return;
}



void ___cxa_begin_catch(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f10. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_begin_catch_000050d8)();
  return;
}



void ___cxa_end_catch(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f16. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_end_catch_000050e0)();
  return;
}



void ___cxa_free_exception(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f1c. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_free_exception_000050e8)();
  return;
}



void ___cxa_throw(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f22. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____cxa_throw_000050f0)();
  return;
}



void ___stack_chk_fail(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f28. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR____stack_chk_fail_000050f8)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

void * _malloc(size_t param_1)

{
  void *pvVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003f2e. Too many branches
                    // WARNING: Treating indirect jump as call
  pvVar1 = (void *)(*(code *)PTR__malloc_00005100)();
  return pvVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

int _memcmp(void *param_1,void *param_2,size_t param_3)

{
  int iVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003f34. Too many branches
                    // WARNING: Treating indirect jump as call
  iVar1 = (*(code *)PTR__memcmp_00005108)();
  return iVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

void * _memcpy(void *param_1,void *param_2,size_t param_3)

{
  void *pvVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003f3a. Too many branches
                    // WARNING: Treating indirect jump as call
  pvVar1 = (void *)(*(code *)PTR__memcpy_00005110)();
  return pvVar1;
}



void _napi_close_handle_scope(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f40. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_close_handle_scope_00005118)();
  return;
}



void _napi_create_double(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f46. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_double_00005120)();
  return;
}



void _napi_create_error(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f4c. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_error_00005128)();
  return;
}



void _napi_create_external(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f52. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_external_00005130)();
  return;
}



void _napi_create_function(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f58. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_function_00005138)();
  return;
}



void _napi_create_object(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f5e. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_object_00005140)();
  return;
}



void _napi_create_reference(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f64. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_reference_00005148)();
  return;
}



void _napi_create_string_utf8(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f6a. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_string_utf8_00005150)();
  return;
}



void _napi_create_symbol(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f70. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_symbol_00005158)();
  return;
}



void _napi_create_type_error(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f76. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_create_type_error_00005160)();
  return;
}



void _napi_define_properties(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f7c. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_define_properties_00005168)();
  return;
}



void _napi_delete_reference(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f82. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_delete_reference_00005170)();
  return;
}



void _napi_fatal_error(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f88. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_fatal_error_00005178)();
  return;
}



void _napi_get_and_clear_last_exception(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f8e. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_and_clear_last_exception_00005180)();
  return;
}



void _napi_get_cb_info(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f94. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_cb_info_00005188)();
  return;
}



void _napi_get_last_error_info(void)

{
                    // WARNING: Could not recover jumptable at 0x00003f9a. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_last_error_info_00005190)();
  return;
}



void _napi_get_reference_value(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fa0. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_reference_value_00005198)();
  return;
}



void _napi_get_undefined(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fa6. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_undefined_000051a0)();
  return;
}



void _napi_get_value_string_utf16(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fac. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_value_string_utf16_000051a8)();
  return;
}



void _napi_get_value_string_utf8(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fb2. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_get_value_string_utf8_000051b0)();
  return;
}



void _napi_is_exception_pending(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fb8. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_is_exception_pending_000051b8)();
  return;
}



void _napi_module_register(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fbe. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_module_register_000051c0)();
  return;
}



void _napi_open_handle_scope(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fc4. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_open_handle_scope_000051c8)();
  return;
}



void _napi_set_property(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fca. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_set_property_000051d0)();
  return;
}



void _napi_throw(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fd0. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_throw_000051d8)();
  return;
}



void _napi_typeof(void)

{
                    // WARNING: Could not recover jumptable at 0x00003fd6. Too many branches
                    // WARNING: Treating indirect jump as call
  (*(code *)PTR__napi_typeof_000051e0)();
  return;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

int _statvfs(char *param_1,statvfs *param_2)

{
  int iVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003fdc. Too many branches
                    // WARNING: Treating indirect jump as call
  iVar1 = (*(code *)PTR__statvfs_000051e8)();
  return iVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

char * _strcat(char *param_1,char *param_2)

{
  char *pcVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003fe2. Too many branches
                    // WARNING: Treating indirect jump as call
  pcVar1 = (char *)(*(code *)PTR__strcat_000051f0)();
  return pcVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

char * _strcpy(char *param_1,char *param_2)

{
  char *pcVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003fe8. Too many branches
                    // WARNING: Treating indirect jump as call
  pcVar1 = (char *)(*(code *)PTR__strcpy_000051f8)();
  return pcVar1;
}



// WARNING: Unknown calling convention -- yet parameter storage is locked

size_t _strlen(char *param_1)

{
  size_t sVar1;
  
                    // WARNING: Could not recover jumptable at 0x00003fee. Too many branches
                    // WARNING: Treating indirect jump as call
  sVar1 = (*(code *)PTR__strlen_00005200)();
  return sVar1;
}



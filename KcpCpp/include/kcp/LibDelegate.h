#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_DELEGATE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_DELEGATE_H__

#pragma once

#include "kcp/common/common.h"

KCP_CPP_BEGIN

// R回调返回值类型，Args回调函数参数包 委托基类 用于解耦具体类型，创建类型无关的委托
template <typename Rtn, typename... Args>
class IDelegate
{
public:
    IDelegate();
    virtual ~IDelegate();
    // 左值会绑定成左值引用，右值会绑定成右值引用
    // 请注意引用折叠适当使用std::forward可以完美的将参数传入，原来什么类型传入后绑定的就是什么类型
    virtual Rtn Invoke(Args... args) = 0;   // 为什么不使用 && 因为为了匹配被委托的参数类型, 只要内部使用std::forward即可完美转发
    virtual Rtn Invoke(Args... args) const = 0;
    virtual IDelegate<Rtn, Args...> *CreateNewCopy() const = 0;
    virtual void Release();
};

////////////////////
// 类委托
template <typename ObjType, typename Rtn, typename... Args>
class DelegateClass : public IDelegate<Rtn, Args...>
{
public:
    DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...));
    DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...) const);
    virtual ~DelegateClass();

    virtual Rtn Invoke(Args... args);
    virtual Rtn Invoke(Args... args) const;
    virtual IDelegate<Rtn, Args...> *CreateNewCopy() const;

private:
    mutable ObjType *_obj;
    mutable Rtn(ObjType::*_f)(Args...);
    // mutable R(T::*_fconst)(Args...) const;
};


////////////////////
// 帮助释放obj,但是不可以创建obj的副本,避免不可控
template <typename ObjType, typename Rtn, typename... Args>
class DelegateClassDelObj : public IDelegate<Rtn, Args...>
{
public:
    DelegateClassDelObj(ObjType *t, Rtn(ObjType::*f)(Args...));
    DelegateClassDelObj(ObjType *t, Rtn(ObjType::*f)(Args...) const);
    virtual ~DelegateClassDelObj();

    virtual Rtn Invoke(Args... args);
    virtual Rtn Invoke(Args... args) const;

    // 禁用拷贝与赋值，以及创建副本
private:
    DelegateClassDelObj(const DelegateClassDelObj<ObjType, Rtn, Args...> &) {}
    DelegateClassDelObj<ObjType, Rtn, Args...> &operator=(const DelegateClass<ObjType, Rtn, Args...> &) {}
    virtual IDelegate<Rtn, Args...> *CreateNewCopy() const { return NULL; }

private:
    mutable ObjType * _obj;
    mutable Rtn(ObjType::*_f)(Args...);
    // mutable R(T::*_fconst)(Args...) const;
};


////////////////////
// 函数委托
template <typename Rtn, typename... Args>
class DelegateFunction : public IDelegate<Rtn, Args...>
{
public:
    DelegateFunction(Rtn(*f)(Args...));
    virtual ~DelegateFunction();

    virtual Rtn Invoke(Args... args);
    virtual Rtn Invoke(Args... args) const;
    virtual IDelegate<Rtn, Args...> *CreateNewCopy() const;

private:
    mutable Rtn(*_f)(Args...);
};


////////////////////
// 支持lambda表达式,std::function等闭包委托
template <typename ClosureFuncType, typename Rtn, typename... Args>
class DelegateClosureFunc : public IDelegate<Rtn, Args...>
{
public:
    DelegateClosureFunc(ClosureFuncType &&closureFunc);
    DelegateClosureFunc(ClosureFuncType const&closureFunc);
    virtual ~DelegateClosureFunc();

    virtual Rtn Invoke(Args... args);
    virtual Rtn Invoke(Args... args) const;
    virtual IDelegate<Rtn, Args...> *CreateNewCopy() const;

private:
    mutable ClosureFuncType _closureFun;
};


////////////////////
// 委托工厂
class DelegateFactory
{
public:
    // 委托,委托释放时不释放obj对象
    template <typename ObjType, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(ObjType *obj, Rtn(ObjType::*f)(Args...));
    template <typename ObjType, typename Rtn, typename... Args>
    static const IDelegate<Rtn, Args...> *Create(ObjType *obj, Rtn(ObjType::*f)(Args...) const);

    // 委托,委托释放时候将释放obj对象 TODO:待测试,确认obj是否被释放
    template <typename ObjType, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *CreateAndHelpDelObj(ObjType *obj, Rtn(ObjType::*f)(Args...));
    template <typename ObjType, typename Rtn, typename... Args>
    static const IDelegate<Rtn, Args...> *CreateAndHelpDelObj(ObjType *obj, Rtn(ObjType::*f)(Args...) const);

    // 普通函数
    template <typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(Rtn(*f)(Args...));

    // 绑定lambda,std::function,如：DelegateFactory::Create<decltype(func), void, int>(func);
    template <typename ClosureFuncType /* = decltype(func) */, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(ClosureFuncType &&func);

    // 绑定lambda,std::function,如：DelegateFactory::Create<decltype(func), void, int>(func);
    template <typename ClosureFuncType /* = decltype(func) */, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(ClosureFuncType const&func);
};

KCP_CPP_END

// 闭包委托创建
#undef CREATE_CLOSURE_DELEGATE
#define CREATE_CLOSURE_DELEGATE(closureTypeFunc, Rtn, ...)    \
KCP_CPP_NS::DelegateFactory::Create<decltype(closureTypeFunc), Rtn, ##__VA_ARGS__>(closureTypeFunc)

#include "kcp/LibDelegateimpl.h"

#endif

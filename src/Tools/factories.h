#ifndef _FACTORIES_H_
#define _FACTORIES_H_

namespace Tools {

    template <typename T, typename ArgType = void>
    struct Types 
    {
        typedef QSharedPointer<T> Spt;
        typedef QWeakPointer<T> Wpt;
        virtual ~Types() {}
    };

    /// Factory template
    template <typename T>
    struct CreateSimple 
    {
        static typename T::Spt create() { return typename T::Spt( new T()); }
    }; 

    /// Factory template
    template <typename T,typename ArgType>
    struct CreateWithArg 
    {
        static typename T::Spt create( ArgType arg ) { return typename T::Spt( new T( arg )); }
    };

    /// Factory template
    template <typename T,typename ArgType_0, typename ArgType_1 >
    struct CreateWithTwoArgs 
    {
        static typename T::Spt create( ArgType_0 arg0 , ArgType_1 arg1) { return typename T::Spt( new T( arg0, arg1 )); }
    };

}

#endif /* _FACTORIES_H_ */

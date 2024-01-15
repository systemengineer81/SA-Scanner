
#ifndef __EVENT_H__
#define __EVENT_H__


namespace Sharp    // short for SharpTools
{

/**
* base class for all event handlers
* event handlers are used internally by Event when EventHandler::Bind() is called.
* @note : You won't create one yourself
* @author Amer Saffo
*/
class EventHandlerImplBase
{
public:
    EventHandlerImplBase() {}
    virtual ~EventHandlerImplBase() {} ///< destructor should be made virtual for base as we are going to delete through a base pointer

};

//------------------------------------
// one argument event handlers support
//------------------------------------

template<typename T>
class EventHandlerImpl: public EventHandlerImplBase
{
public:
    virtual void OnEvent(T&) = 0;  ///< will be called eventually when a Event is raised
};
    
/** A handler non-member function calls */
template<typename T>
class EventHandlerImplStatic : public EventHandlerImpl<T>
{
public:
    /** Saves the passed function for use later when an event is raised */
    EventHandlerImplStatic(void(*functionToCall)(T&))
        : m_pFunctionToCall(functionToCall)
    {
    }

    /** will be called eventually when an Event is raised */
    virtual void OnEvent(T& evt)
    {
        m_pFunctionToCall(evt);
    }

private:

    void (*m_pFunctionToCall)(T &); ///< passed in the constructor. Will get called when an event is raised.
};

/** A helper that handles member function calls */
template<typename T, typename Class>
class EventHandlerImplMember : public EventHandlerImpl<T>
{
private:
    Class* m_pThis;

    void (Class::*m_pMemberFunction)(T&);

public:
    EventHandlerImplMember(void(Class::*memberFunctionToCall)(T&), Class* This)
        : m_pThis(This)
        , m_pMemberFunction(memberFunctionToCall)
    {
    }

    virtual void OnEvent(T& evt)
    {
        if (m_pThis)
        {
            (m_pThis->*m_pMemberFunction)(evt);
        }
    }
};
    
//------------------------------------
// no arguments event handlers support
//------------------------------------

template<>
class EventHandlerImpl<void> : public EventHandlerImplBase
{
/**
* a specialization of the EventHandlerImpl for when void is passed
* which happen when user define an Event<void>
*/
public:
    virtual void OnEvent() = 0;
};

class EventHandlerImplForStatic : public EventHandlerImpl<void>
{
/** A handler non-member void function calls */
private:
    void (*m_pFunctionToCall)(); ///< passed in the constructor. Will get called when an event is raised.

public:
    /** Saves the passed function for use later when an event is raised */
    EventHandlerImplForStatic(void(*functionToCall)())
        : m_pFunctionToCall(functionToCall)
    {
    }

    /** will be called eventually when an Event is raised */
    virtual void OnEvent()
    {
        m_pFunctionToCall();
    }
};

/** A helper that handles void member function calls */
template<typename Class>
class EventHandlerImplMember<void, Class> : public EventHandlerImpl<void>
{
private:
    Class    *m_pThis;  ///< passed in the constructor. This watcher will only be used to call a member function, so m_pCallerInstance would hold the object through which that member function is called.
    void (Class::*m_pMemberFunction)(); ///< passed in the constructor. This watcher will only be used to call a member function through m_pCallerInstance.

public:
    /** Saves the passed function for use later when an event arrive */
    EventHandlerImplMember(void(Class::*memberFunctionToCall)(), Class* This)
        : m_pThis(This)
        , m_pMemberFunction(memberFunctionToCall)
    {
    }

    /** will be called eventually when an Event is raised */
    virtual void OnEvent()
    {
        if (m_pThis)
        {
            (m_pThis->*m_pMemberFunction)();
        }
    }
};

    

/** a syntax-sugar class that is used to assign a function as a handler to an Event
* @example myEvent += EventHandler::Bind(OnMessage);  // for non member or static functions
* @example myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);  // called inside ThisClass constructor as recommended
* @author Amer Saffo
*/
class EventHandler
{
public:

    //------------------------------------
    // one argument event handlers support
    //------------------------------------

    /**
    * to be used when setting the event handler function to an event
    * the event data type T should be the same as the one used to create the Event or the compiler will complain
    * that the the event can't accept the binded handler
    * @note : EventHandler::Bind() was designed to be used ONLY to set event handlers to Event. // myEvent += EventHandler::Bind(...)
    *         It is important to note that Event will own the memory created by Bind when the created event handler is assigned to it by its += call,
    *         and so Event would destroy that memory when it no longer need it. So make sure not to keep the returned pointer yourself.
    */
    template<typename T>
    static EventHandlerImpl<T>* Bind(void(*nonMemberFunctionToCall)(T&))
    {
        return new EventHandlerImplForStatic<T>(nonMemberFunctionToCall);
    }

    /** @overload */
    template<typename T, typename U>
    static EventHandlerImpl<T>* Bind(void(U::*memberFunctionToCall)(T&), U* thisPtr)
    {
        return new EventHandlerImplMember<T,U>(memberFunctionToCall, thisPtr);
    }

    //------------------------------------
    // no arguments event handlers support
    //------------------------------------
        
    /** @overload */
    static EventHandlerImpl<void>* Bind(void(*nonMemberFunctionToCall)())
    {
        return new EventHandlerImplForStatic(nonMemberFunctionToCall);
    }

    /** @overload */
    template<typename U>
    static EventHandlerImpl<void>* Bind(void(U::*memberFunctionToCall)(), U* thisPtr)
    {
        return new EventHandlerImplMember<void, U>(memberFunctionToCall, thisPtr);
    }

private:
    EventHandler();  ///< default constructor made private to prevent creating instances of this class. EventHandler only purpose is to provide Event with the Bind function
};



/**
* Sharp Event provide an event mechanism that is similar to that found in C#
* @example myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);  // called inside ThisClass constructor as recommended	
* @author Amer Saffo
*/
template<typename T>
class EventBase
{

public:
    typedef struct _DATA_ENTRY {
        LIST_ENTRY           Entry;
        void                *pHandler;  
    } DATA_ENTRY, *PDATA_ENTRY;

protected:

    LIST_ENTRY m_eventHead;  ///< all handlers will be notified when operator() is called.

public:
    EventBase()
    {
        InitializeListHead(&m_eventHead);
    }
        
    /**
    * it is by design that Event is the owner of the memory of all the handlers assigned to it
    * so it is the duty of the destructor to erase that memory
    * @see Eventhandler Bind function documentation for more details
    */
    virtual ~EventBase()
    {

        while (!IsListEmpty(&m_eventHead))
        {
            LIST_ENTRY *ListEntry;
            EventHandlerImpl<T>* pHandler;

            ListEntry = RemoveHeadList(&m_eventHead);


            DATA_ENTRY *pData = CONTAINING_RECORD(ListEntry, DATA_ENTRY, Entry);

            pHandler = (EventHandlerImpl<T>*)pData->pHandler;

            delete pHandler;
            delete pData;
        }

    }

    /**
    * This is how you connect a handler to this event.
    * @example myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);  // example of binding inside ThisClass constructor as recommended	
    *	@example myEvent -= EventHandler::Bind(&ThisClass::OnMessage, this);  // example of unbinding in destructor
    * @note : You can Bind to a private member function as long as you have access to that private function at the time of binding (i.e binding from within the class).
    * @note on memory: As a rule of thumb, always bind in the constructor and unbind in the destructor.
    *									 However, you can go lazy and rely on the Event destructor to unbind if:
    *									 [1] if you are passing [this] to EventHandler::Bind() AND the event is a member, which is the most common scenario.
    *											 as that means the member event object will be automatically destroyed by the class destructor.
    *									 [2] or if you binded to a non member function, as those are never destroyed.
    *	In short, before your binded class instance is destroyed, make sure to unbind it. Otherwise, the binded event might try to make a call through your destroyed instance.
    * @note : For completeness, you are warned NOT to store the returned value of Eventhandler::Bind() yourself, as after this call, Event becomes the owner of the implicitly created EventHandlerImpl<T> and it later destroys it.
    */
    EventBase<T>& operator += (EventHandlerImpl<T>* pHandlerToAdd)
    {
        // bellow is commented because we decided to let the user add the same handler multiple time and make it his responsibility to remove all those added
        //if( FindHandlerWithSameBinding(pHandlerToAdd) != m_eventHandlers.end())

        if (pHandlerToAdd)
        {

            PDATA_ENTRY  *pEntry = new DATA_ENTRY;

            pEntry->pHandler = pHandlerToAdd;

            InsertTailList(&m_eventHead, &pEntry->Entry);

            // the handler added bellow along with all handlers in the list will be called later when an event is raised
            //m_eventHandlers.push_back(pHandlerToAdd);
        }

        return *this;
    }

    /**
    * you can use this to remove a handler you previously added.
    * @note : removing a handler that was already removed is harmless, as this call does nothing and simply return when it does not find the handler.
    *	@example myEvent -= EventHandler::Bind(&ThisClass::OnMessage, this);  // example of unbinding in destructor
    */
    EventBase<T>& operator -= (EventHandlerImpl<T>* pHandlerToRemove)
    {
        if (!pHandlerToRemove)
        {
            return *this;  // a null passed, so nothing to do
        }
            

        // search for a handler that has the same binding as the passed one
        // search linearly (no other way)
        /*for ( std::list< EventHandlerImpl<T>* >::iterator iter= m_eventHandlers.begin(); iter != m_eventHandlers.end(); ++iter)
        {
            EventHandlerImpl<T>* pHandler= *iter;

            // erase the memory that was created by the Bind function
            // this memory is that of an EventHandler class and has nothing to do with the actual functions/class passed to it on Bind
            EventHandlerImpl<T>* pFoundHandler= *iter;
            if (pFoundHandler)
            {
                delete pFoundHandler;
                pFoundHandler= 0;
            }

            // remove it form the list (safe to do it here as we'll break the loop)
            m_eventHandlers.erase(iter);
            break;
        }*/

        // also delete the passed handler as we don't need it anymore (by design, Event always owns the memory of the handlers passed to it)
        /*if (pHandlerToRemove)
        {
            delete pHandlerToRemove;
            pHandlerToRemove= 0;
        }*/
            
        return *this;
    }

private:
    EventBase(const EventBase &);            // < private to disable copying
    EventBase& operator= (const EventBase &); // < private to disable copying
};

//------------------------------------
// one argument event handlers support
//------------------------------------
template<typename T>
class Event : public EventBase<T>
{
public:
    /**
    * this call actually raises the event.
    * It does so by passing the event data to all event handlers.
    * @example @code myEvent(data);  // this how you would normally raise an event
    */
    void operator()(T& eventData)
    {
        //std::list<void *>::iterator iter = this->m_eventHandlers.begin();

        LIST_ENTRY *ListEntry;

        ListEntry = this->m_eventHead.Flink;

        while (ListEntry != &this->m_eventHead)
        {

            EventHandlerImpl<T>* pHandler;

            EventBase::DATA_ENTRY *pData = CONTAINING_RECORD(ListEntry, EventBase::DATA_ENTRY, Entry);

            pHandler = (EventHandlerImpl<T>* )pData->pHandler;

            if (pHandler)
            {
                pHandler->OnEvent(eventData);  // this is a virtual function that will eventually call the function passed to Eventhandler::Bind() for this handler
            }

            ListEntry = ListEntry->Flink;
        }
    }
};

//----------------------------
// no arguments event handlers support
//----------------------------
template<>
class Event<void> : public EventBase<void>
{
public:
    /**
    * this call actually raises the event.
    * It does so by calling all event handlers.
    * @example @code myEvent();  // this how you would normally raise a void event
    */
    void operator()()
    {

        LIST_ENTRY *Entry;

        Entry = this->m_eventHead.Flink;

        while (Entry != &this->m_eventHead)
        {

            EventHandlerImpl<void>* pHandler;

            pHandler = (EventHandlerImpl<void>*)CONTAINING_RECORD(Entry, DATA_ENTRY, Entry);

            if (pHandler)
            {
                pHandler->OnEvent();  // this is a virtual function that will eventually call the function passed to Eventhandler::Bind() for this handler
            }

            Entry = Entry->Flink;
        }

    }
};

};

#endif // SHARP_EVENT_HEADER__
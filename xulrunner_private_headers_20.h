#ifndef __WXWEBCONNECT_XULRUNNER_PRIVATE_HEADERS_192_H
#define __WXWEBCONNECT_XULRUNNER_PRIVATE_HEADERS_192_H

///////////////////////////////////////////////////////////////////////////////
//  nsEventStatus (from nsEvent.h).
///////////////////////////////////////////////////////////////////////////////

/**
 * Return status for event processors.
 */

enum nsEventStatus {  
    /// The event is ignored, do default processing
  nsEventStatus_eIgnore,            
    /// The event is consumed, don't do default processing
  nsEventStatus_eConsumeNoDefault, 
    /// The event is consumed, but do default processing
  nsEventStatus_eConsumeDoDefault  
};

///////////////////////////////////////////////////////////////////////////////
//  nsIScriptGlobalObject
///////////////////////////////////////////////////////////////////////////////

class nsIScriptContext;
class nsIDOMDocument;
class nsIDOMEvent;
class nsIScriptGlobalObjectOwner;
class nsIArray;
class nsScriptErrorEvent;
class nsIScriptGlobalObject;
struct JSObject; // until we finally remove GetGlobalJSObject...

// Some helpers for working with integer "script type IDs", and specifically
// for working with arrays of such objects. For example, it is common for
// implementations supporting multiple script languages to keep each
// language's nsIScriptContext in an array indexed by the language ID.

// Implementation note: We always ignore nsIProgrammingLanguage::UNKNOWN and
// nsIProgrammingLanguage::CPLUSPLUS - this gives javascript slot 0.  An
// attempted micro-optimization tried to avoid us going all the way to 
// nsIProgrammingLanguage::MAX; however:
// * Someone is reportedly working on a PHP impl - that has value 9
// * nsGenericElement therefore allows 4 bits for the value.
// So there is no good reason for us to be more restrictive again...

#define NS_STID_FIRST nsIProgrammingLanguage::JAVASCRIPT
// like nsGenericElement, only 4 bits worth is valid...
#define NS_STID_LAST (nsIProgrammingLanguage::MAX > 0x000FU ? \
                      0x000FU : nsIProgrammingLanguage::MAX)

// Use to declare the array size
#define NS_STID_ARRAY_UBOUND (NS_STID_LAST-NS_STID_FIRST+1)

// Is a language ID valid?
#define NS_STID_VALID(langID) (langID >= NS_STID_FIRST && langID <= NS_STID_LAST)

// Return an index for a given ID.
#define NS_STID_INDEX(langID) (langID-NS_STID_FIRST)

// Create a 'for' loop iterating over all possible language IDs (*not* indexes)
#define NS_STID_FOR_ID(varName) \
          for (varName=NS_STID_FIRST;varName<=NS_STID_LAST;varName++)

// Create a 'for' loop iterating over all indexes (when you don't need to know
// what language it is)
#define NS_STID_FOR_INDEX(varName) \
          for (varName=0;varName<=NS_STID_INDEX(NS_STID_LAST);varName++)

// A helper function for nsIScriptGlobalObject implementations to use
// when handling a script error.  Generally called by the global when a context
// notifies it of an error via nsIScriptGlobalObject::HandleScriptError.
// Returns PR_TRUE if HandleDOMEvent was actually called, in which case
// aStatus will be filled in with the status.
PRBool
NS_HandleScriptError(nsIScriptGlobalObject *aScriptGlobal,
                     nsScriptErrorEvent *aErrorEvent,
                     nsEventStatus *aStatus);


#define NS_ISCRIPTGLOBALOBJECT_IID \
{ 0xe9f3f2c1, 0x2d94, 0x4722, \
  { 0xbb, 0xd4, 0x2b, 0xf6, 0xfd, 0xf4, 0x2f, 0x48 } }

/**
  * The global object which keeps a script context for each supported script
  * language. This often used to store per-window global state.
 */

class nsIScriptGlobalObject : public nsISupports
{
public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISCRIPTGLOBALOBJECT_IID)

  /**
   * Ensure that the script global object is initialized for working with the
   * specified script language ID.  This will set up the nsIScriptContext
   * and 'script global' for that language, allowing these to be fetched
   * and manipulated.
   * @return NS_OK if successful; error conditions include that the language
   * has not been registered, as well as 'normal' errors, such as
   * out-of-memory
   */
  virtual nsresult EnsureScriptEnvironment(PRUint32 aLangID) = 0;
  /**
   * Get a script context (WITHOUT added reference) for the specified language.
   */
  virtual nsIScriptContext *GetScriptContext(PRUint32 lang) = 0;
  
  /**
   * Get the opaque "global" object for the specified lang.
   */
  virtual void *GetScriptGlobal(PRUint32 lang) = 0;

  // Set/GetContext deprecated methods - use GetScriptContext/Global
  virtual JSObject *GetGlobalJSObject() {
        return (JSObject *)GetScriptGlobal(nsIProgrammingLanguage::JAVASCRIPT);
  }

  virtual nsIScriptContext *GetContext() {
        return GetScriptContext(nsIProgrammingLanguage::JAVASCRIPT);
  }

  /**
   * Set a new language context for this global.  The native global for the
   * context is created by the context's GetNativeGlobal() method.
   */

  virtual nsresult SetScriptContext(PRUint32 lang, nsIScriptContext *aContext) = 0;

  /**
   * Called when the global script for a language is finalized, typically as
   * part of its GC process.  By the time this call is made, the
   * nsIScriptContext for the language has probably already been removed.
   * After this call, the passed object is dead - which should generally be the
   * same object the global is using for a global for that language.
   */

  virtual void OnFinalize(PRUint32 aLangID, void *aScriptGlobal) = 0;

  /**
   * Called to enable/disable scripts.
   */
  virtual void SetScriptsEnabled(PRBool aEnabled, PRBool aFireTimeouts) = 0;

  /**
   * Handle a script error.  Generally called by a script context.
   */
  virtual nsresult HandleScriptError(nsScriptErrorEvent *aErrorEvent,
                                     nsEventStatus *aEventStatus) {
    return NS_HandleScriptError(this, aErrorEvent, aEventStatus);
  }
};

//NS_DEFINE_STATIC_IID_ACCESSOR(nsIScriptGlobalObject,
//                              NS_ISCRIPTGLOBALOBJECT_IID)

///////////////////////////////////////////////////////////////////////////////
//  nsIScriptContext
///////////////////////////////////////////////////////////////////////////////

class nsIScriptGlobalObject;
class nsIScriptSecurityManager;
class nsIPrincipal;
class nsIAtom;
class nsIArray;
class nsIVariant;
class nsIObjectInputStream;
class nsIObjectOutputStream;
class nsScriptObjectHolder;
class nsIScriptObjectPrincipal;

typedef void (*nsScriptTerminationFunc)(nsISupports* aRef);

#define NS_ISCRIPTCONTEXTPRINCIPAL_IID \
  { 0xd012cdb3, 0x8f1e, 0x4440, \
    { 0x8c, 0xbd, 0x32, 0x7f, 0x98, 0x1d, 0x37, 0xb4 } }

class nsIScriptContextPrincipal : public nsISupports
{
public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISCRIPTCONTEXTPRINCIPAL_IID)

  virtual nsIScriptObjectPrincipal* GetObjectPrincipal() = 0;
};

//NS_DEFINE_STATIC_IID_ACCESSOR(nsIScriptContextPrincipal,
//                              NS_ISCRIPTCONTEXTPRINCIPAL_IID)

// a7139c0e-962c-44b6-bec3-e4166bfe84eb
#define NS_ISCRIPTCONTEXT_IID \
{ 0xa7139c0e, 0x962c, 0x44b6, \
  { 0xbe, 0xc3, 0xe4, 0x16, 0x6b, 0xfe, 0x84, 0xeb } }

/* This MUST match JSVERSION_DEFAULT.  This version stuff if we don't
   know what language we have is a little silly... */
#define SCRIPTVERSION_DEFAULT JSVERSION_DEFAULT

/**
 * It is used by the application to initialize a runtime and run scripts.
 * A script runtime would implement this interface.
 * <P><I>It does have a bit too much java script information now, that
 * should be removed in a short time. Ideally this interface will be
 * language neutral</I>
 */
class nsIScriptContext : public nsIScriptContextPrincipal
{
public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISCRIPTCONTEXT_IID)

  /* Get the ID of this language. */
  virtual PRUint32 GetScriptTypeID() = 0;

  /**
   * Compile and execute a script.
   *
   * @param aScript a string representing the script to be executed
   * @param aScopeObject a script object for the scope to execute in, or
   *                     nsnull to use a default scope
   * @param aPrincipal the principal that produced the script
   * @param aURL the URL or filename for error messages
   * @param aLineNo the starting line number of the script for error messages
   * @param aVersion the script language version to use when executing
   * @param aRetValue the result of executing the script, or null for no result.
   *        If this is a JS context, it's the caller's responsibility to
   *        preserve aRetValue from GC across this call
   * @param aIsUndefined true if the result of executing the script is the
   *                     undefined value
   *
   * @return NS_OK if the script was valid and got executed
   *
   **/
  virtual nsresult EvaluateString(const nsAString& aScript,
                                  void *aScopeObject,
                                  nsIPrincipal *aPrincipal,
                                  const char *aURL,
                                  PRUint32 aLineNo,
                                  PRUint32 aVersion,
                                  nsAString *aRetValue,
                                  PRBool* aIsUndefined) = 0;

  // Note JS bigotry remains here - 'void *aRetValue' is assumed to be a
  // jsval.  This must move to JSObject before it can be made agnostic.
  virtual nsresult EvaluateStringWithValue(const nsAString& aScript,
                                           void *aScopeObject,
                                           nsIPrincipal *aPrincipal,
                                           const char *aURL,
                                           PRUint32 aLineNo,
                                           PRUint32 aVersion,
                                           void* aRetValue,
                                           PRBool* aIsUndefined) = 0;

  /**
   * Compile a script.
   *
   * @param aText a PRUnichar buffer containing script source
   * @param aTextLength number of characters in aText
   * @param aScopeObject an object telling the scope in which to execute,
   *                     or nsnull to use a default scope
   * @param aPrincipal the principal that produced the script
   * @param aURL the URL or filename for error messages
   * @param aLineNo the starting line number of the script for error messages
   * @param aVersion the script language version to use when executing
   * @param aScriptObject an executable object that's the result of compiling
   *                      the script.
   *
   * @return NS_OK if the script source was valid and got compiled.
   *
   **/
  virtual nsresult CompileScript(const PRUnichar* aText,
                                 PRInt32 aTextLength,
                                 void* aScopeObject,
                                 nsIPrincipal* aPrincipal,
                                 const char* aURL,
                                 PRUint32 aLineNo,
                                 PRUint32 aVersion,
                                 nsScriptObjectHolder &aScriptObject) = 0;

  /**
   * Execute a precompiled script object.
   *
   * @param aScriptObject an object representing the script to be executed
   * @param aScopeObject an object telling the scope in which to execute,
   *                     or nsnull to use a default scope
   * @param aRetValue the result of executing the script, may be null in
   *                  which case no result string is computed
   * @param aIsUndefined true if the result of executing the script is the
   *                     undefined value, may be null for "don't care"
   *
   * @return NS_OK if the script was valid and got executed
   *
   */
  virtual nsresult ExecuteScript(void* aScriptObject,
                                 void* aScopeObject,
                                 nsAString* aRetValue,
                                 PRBool* aIsUndefined) = 0;

  /**
   * Compile the event handler named by atom aName, with function body aBody
   * into a function object returned if ok via aHandler.  Does NOT bind the
   * function to anything - BindCompiledEventHandler() should be used for that
   * purpose.  Note that this event handler is always considered 'shared' and
   * hence is compiled without principals.  Never call the returned object
   * directly - it must be bound (and thereby cloned, and therefore have the 
   * correct principals) before use!
   *
   * If the compilation sets a pending exception on the native context, it is
   * this method's responsibility to report said exception immediately, without
   * relying on callers to do so.
   *
   *
   * @param aName an nsIAtom pointer naming the function; it must be lowercase
   *        and ASCII, and should not be longer than 63 chars.  This bound on
   *        length is enforced only by assertions, so caveat caller!
   * @param aEventName the name that the event object should be bound to
   * @param aBody the event handler function's body
   * @param aURL the URL or filename for error messages
   * @param aLineNo the starting line number of the script for error messages
   * @param aVersion the script language version to use when executing
   * @param aHandler the out parameter in which a void pointer to the compiled
   *        function object is stored on success
   *
   * @return NS_OK if the function body was valid and got compiled
   */
  virtual nsresult CompileEventHandler(nsIAtom* aName,
                                       PRUint32 aArgCount,
                                       const char** aArgNames,
                                       const nsAString& aBody,
                                       const char* aURL,
                                       PRUint32 aLineNo,
                                       PRUint32 aVersion,
                                       nsScriptObjectHolder &aHandler) = 0;

  /**
   * Call the function object with given args and return its boolean result,
   * or true if the result isn't boolean.
   *
   * @param aTarget the event target
   * @param aScript an object telling the scope in which to call the compiled
   *        event handler function.
   * @param aHandler function object (function and static scope) to invoke.
   * @param argv array of arguments.  Note each element is assumed to
   *        be an nsIVariant.
   * @param rval out parameter returning result
   **/
  virtual nsresult CallEventHandler(nsISupports* aTarget,
                                    void *aScope, void* aHandler,
                                    nsIArray *argv, nsIVariant **rval) = 0;

  /**
   * Bind an already-compiled event handler function to a name in the given
   * scope object.  The same restrictions on aName (lowercase ASCII, not too
   * long) applies here as for CompileEventHandler.  Scripting languages with
   * static scoping must re-bind the scope chain for aHandler to begin (after
   * the activation scope for aHandler itself, typically) with aTarget's scope.
   *
   * Logically, this 'bind' operation is more of a 'copy' - it simply
   * stashes/associates the event handler function with the event target, so
   * it can be fetched later with GetBoundEventHandler().
   *
   * @param aTarget an object telling the scope in which to bind the compiled
   *        event handler function.  The context will presumably associate
   *        this nsISupports with a native script object.
   * @param aName an nsIAtom pointer naming the function; it must be lowercase
   *        and ASCII, and should not be longer than 63 chars.  This bound on
   *        length is enforced only by assertions, so caveat caller!
   * @param aHandler the function object to name, created by an earlier call to
   *        CompileEventHandler
   * @return NS_OK if the function was successfully bound to the name
   *
   * XXXmarkh - fold this in with SetProperty?  Exactly the same concept!
   */
  virtual nsresult BindCompiledEventHandler(nsISupports* aTarget, void *aScope,
                                            nsIAtom* aName,
                                            void* aHandler) = 0;

  /**
   * Lookup a previously bound event handler for the specified target.  This
   * will return an object equivilent to the one passed to
   * BindCompiledEventHandler (although the pointer may not be the same).
   *
   */
  virtual nsresult GetBoundEventHandler(nsISupports* aTarget, void *aScope,
                                        nsIAtom* aName,
                                        nsScriptObjectHolder &aHandler) = 0;

  /**
   * Compile a function that isn't used as an event handler.
   *
   * NOTE: Not yet language agnostic (main problem is XBL - not yet agnostic)
   * Caller must make sure aFunctionObject is a JS GC root.
   *
   **/
  virtual nsresult CompileFunction(void* aTarget,
                                   const nsACString& aName,
                                   PRUint32 aArgCount,
                                   const char** aArgArray,
                                   const nsAString& aBody,
                                   const char* aURL,
                                   PRUint32 aLineNo,
                                   PRUint32 aVersion,
                                   PRBool aShared,
                                   void **aFunctionObject) = 0;

  /**
   * Set the default scripting language version for this context, which must
   * be a context specific to a particular scripting language.
   *
   **/
  virtual void SetDefaultLanguageVersion(PRUint32 aVersion) = 0;

  /**
   * Return the global object.
   *
   **/
  virtual nsIScriptGlobalObject *GetGlobalObject() = 0;

  /**
   * Return the native script context
   *
   **/
  virtual void *GetNativeContext() = 0;

  /**
   * Return the native global object for this context.
   *
   **/
  virtual void *GetNativeGlobal() = 0;

  /**
   * Create a new global object that will be used for an inner window.
   * Return the native global and an nsISupports 'holder' that can be used
   * to manage the lifetime of it.
   */
  virtual nsresult CreateNativeGlobalForInner(
                                      nsIScriptGlobalObject *aNewInner,
                                      PRBool aIsChrome,
                                      nsIPrincipal *aPrincipal,
                                      void **aNativeGlobal,
                                      nsISupports **aHolder) = 0;

  /**
   * Connect this context to a new inner window, to allow "prototype"
   * chaining from the inner to the outer.
   * Called after both the the inner and outer windows are initialized
   **/
  virtual nsresult ConnectToInner(nsIScriptGlobalObject *aNewInner,
                                  void *aOuterGlobal) = 0;


  /**
   * Initialize the context generally. Does not create a global object.
   **/
  virtual nsresult InitContext() = 0;

  /**
   * Creates the outer window for this context.
   *
   * @param aGlobalObject The script global object to use as our global.
   */
  virtual nsresult CreateOuterObject(nsIScriptGlobalObject *aGlobalObject,
                                     nsIScriptGlobalObject *aCurrentInner) = 0;

  /**
   * Given an outer object, updates this context with that outer object.
   */
  virtual nsresult SetOuterObject(void *aOuterObject) = 0;

  /**
   * Prepares this context for use with the current inner window for the
   * context's global object. This must be called after CreateOuterObject.
   */
  virtual nsresult InitOuterWindow() = 0;

  /**
   * Check to see if context is as yet intialized. Used to prevent
   * reentrancy issues during the initialization process.
   *
   * @return PR_TRUE if initialized, PR_FALSE if not
   *
   */
  virtual PRBool IsContextInitialized() = 0;

  /**
   * Called as the global object discards its reference to the context.
   */
  virtual void FinalizeContext() = 0;

  /**
   * For garbage collected systems, do a synchronous collection pass.
   * May be a no-op on other systems
   *
   * @return NS_OK if the method is successful
   */
  virtual void GC() = 0;

  /**
   * Inform the context that a script was evaluated.
   * A GC may be done if "necessary."
   * This call is necessary if script evaluation is done
   * without using the EvaluateScript method.
   * @param aTerminated If true then call termination function if it was 
   *    previously set. Within DOM this will always be true, but outside 
   *    callers (such as xpconnect) who may do script evaluations nested
   *    inside DOM script evaluations can pass false to avoid premature
   *    calls to the termination function.
   * @return NS_OK if the method is successful
   */
  virtual void ScriptEvaluated(PRBool aTerminated) = 0;

  virtual nsresult Serialize(nsIObjectOutputStream* aStream,
                             void *aScriptObject) = 0;
  
  /* Deserialize a script from a stream.
   */
  virtual nsresult Deserialize(nsIObjectInputStream* aStream,
                               nsScriptObjectHolder &aResult) = 0;

  /**
   * JS only - this function need not be implemented by languages other
   * than JS (ie, this should be moved to a private interface!)
   * Called to specify a function that should be called when the current
   * script (if there is one) terminates. Generally used if breakdown
   * of script state needs to happen, but should be deferred till
   * the end of script evaluation.
   *
   * @throws NS_ERROR_OUT_OF_MEMORY if that happens
   */
  virtual nsresult SetTerminationFunction(nsScriptTerminationFunc aFunc,
                                          nsISupports* aRef) = 0;

  /**
   * Called to disable/enable script execution in this context.
   */
  virtual PRBool GetScriptsEnabled() = 0;
  virtual void SetScriptsEnabled(PRBool aEnabled, PRBool aFireTimeouts) = 0;

  // SetProperty is suspect and jst believes should not be needed.  Currenly
  // used only for "arguments".
  virtual nsresult SetProperty(void *aTarget, const char *aPropName, nsISupports *aVal) = 0;
  /** 
   * Called to set/get information if the script context is
   * currently processing a script tag
   */
  virtual PRBool GetProcessingScriptTag() = 0;
  virtual void SetProcessingScriptTag(PRBool aResult) = 0;

  /**
   * Called to find out if this script context might be executing script.
   */
  virtual PRBool GetExecutingScript() = 0;

  /**
   * Tell the context whether or not to GC when destroyed.  An optimization
   * used when the window is a [i]frame, so GC will happen anyway.
   */
  virtual void SetGCOnDestruction(PRBool aGCOnDestruction) = 0;

  /**
   * Initialize DOM classes on aGlobalObj, always call
   * WillInitializeContext() before calling InitContext(), and always
   * call DidInitializeContext() when a context is fully
   * (successfully) initialized.
   */
  virtual nsresult InitClasses(void *aGlobalObj) = 0;

  /**
   * Clear the scope object - may be called either as we are being torn down,
   * or before we are attached to a different document.
   *
   * aClearFromProtoChain is probably somewhat JavaScript specific.  It
   * indicates that the global scope polluter should be removed from the
   * prototype chain and that the objects in the prototype chain should
   * also have their scopes cleared.  We don't do this all the time
   * because the prototype chain is shared between inner and outer
   * windows, and needs to stay with inner windows that we're keeping
   * around.
   */
  virtual void ClearScope(void* aGlobalObj, PRBool aClearFromProtoChain) = 0;

  /**
   * Tell the context we're about to be reinitialize it.
   */
  virtual void WillInitializeContext() = 0;

  /**
   * Tell the context we're done reinitializing it.
   */
  virtual void DidInitializeContext() = 0;

  /**
   * Tell the context our global has a new document, and the scope
   * used by it.  Use nsISupports to avoid dependency issues - but expect
   * a QI for nsIDOMDocument and/or nsIDocument.
   */
  virtual void DidSetDocument(nsISupports *aDoc, void *aGlobal) = 0;

  /* Memory managment for script objects.  Used by the implementation of
   * nsScriptObjectHolder to manage the lifetimes of the held script objects.
   *
   * See also nsIScriptRuntime, which has identical methods and is useful
   * in situations when you do not have an nsIScriptContext.
   * 
   */
  virtual nsresult DropScriptObject(void *object) = 0;
  virtual nsresult HoldScriptObject(void *object) = 0;

  virtual void EnterModalState() = 0;
  virtual void LeaveModalState() = 0;
};

#endif

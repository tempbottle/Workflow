/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIME
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIME

#include "../WorkflowVlppReferences.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			struct WfRuntimeThreadContext;
			class IWfDebuggerCallback;
			class WfDebugger;

/***********************************************************************
Instruction
***********************************************************************/

			enum class WfInsCode
			{
				// Instruction		// param				: <Stack-Pattern> -> <Stack-Pattern> in the order of <bottom ---- top>
				Nop,				// 						: () -> ()										;
				LoadValue,			// value				: () -> Value									;
				LoadClosure,		// function, count		: Value-1, ..., Value-count -> Value			;
				LoadException,		// 						: () -> Value									;
				LoadLocalVar,		// variable				: () -> Value									;
				LoadCapturedVar,	// variable				: () -> Value									;
				LoadGlobalVar,		// variable				: () -> Value									;
				StoreLocalVar,		// variable				: Value -> ()									;
				StoreGlobalVar,		// variable				: Value -> ()									;
				Duplicate,			// count				: () -> Value									; copy stack[stack.Count()-1-count]
				Pop,				//						: Value -> ()									;
				Return,				// 						: Value -> Value								; (exit function)
				CreateArray,		// count				: Value-count, ..., Value-1 -> <array>			; {1 2 3} -> <3 2 1>
				CreateMap,			// count				: Value-count, ..., Value-1 -> <map>			; {1:2 3:4} -> <3 4 1 2>
				CreateInterface,	// count				: Value-count, ..., Value-1 -> <map>			; {"Get":a "Set":b} -> InterfaceProxy^
				CreateRange,		// I1248/U1248			: Value-begin, Value-end -> <enumerable>		;
				ReverseEnumerable,	//						: Value -> Value								;
				DeleteRawPtr,		//						: Value -> ()									;
				ConvertToType,		// flag, typeDescriptor	: Value -> Value								;
				TryConvertToType,	// flag, typeDescriptor	: Value -> Value								;
				TestType,			// flag, typeDescriptor	: Value -> <bool>								;
				GetType,			//						: Value -> <ITypeDescriptor*>					;
				Jump,				// label				: () -> ()										;
				JumpIf,				// label				: () -> ()										;
				Invoke,				// function, count		: Value-1, ..., Value-n -> Value				;
				GetProperty,		// IPropertyInfo*		: Value-this -> Value							;
				InvokeProxy,		// count				: Value-1, ..., Value-n, Value-this -> Value	;
				InvokeMethod,		// IMethodInfo*, count	: Value-1, ..., Value-n, Value-this -> Value	;
				AttachEvent,		// IEventInfo*			: Value-this, <function> -> <Listener>			;
				DetachEvent,		// 						: <Listener> -> bool							;
				InstallTry,			// label				: () -> ()										;
				UninstallTry,		// count				: () -> ()										;
				RaiseException,		// 						: Value -> ()									; (trap)
				TestElementInSet,	//						: Value-element, Value-set -> bool				;
				CompareLiteral,		// I48/U48/F48/S		: Value, Value -> <int>							;
				CompareStruct,		// 						: Value, Value -> <bool>						;
				CompareReference,	// 						: Value, Value -> <bool>						;
				CompareValue,		// 						: Value, Value -> <bool>						;
				OpNot,				// B/I1248/U1248		: Value -> Value								;
				OpPositive,			// I1248/U1248			: Value -> Value								;
				OpNegative,			// I1248				: Value -> Value								;
				OpConcat,			// 						: <string>, <string> -> <string>				;
				OpExp,				// I48/U48/F48			: Value, Value -> Value							;
				OpAdd,				// I48/U48/F48			: Value, Value -> Value							;
				OpSub,				// I48/U48/F48			: Value, Value -> Value							;
				OpMul,				// I48/U48/F48			: Value, Value -> Value							;
				OpDiv,				// I48/U48/F48			: Value, Value -> Value							;
				OpMod,				// I48/U48				: Value, Value -> Value							;
				OpShl,				// I48/U48				: Value, Value -> Value							;
				OpShr,				// I48/U48				: Value, Value -> Value							;
				OpXor,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpAnd,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpOr,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpLT,				// 						: <int> -> <bool>								;
				OpGT,				// 						: <int> -> <bool>								;
				OpLE,				// 						: <int> -> <bool>								;
				OpGE,				// 						: <int> -> <bool>								;
				OpEQ,				// 						: <int> -> <bool>								;
				OpNE,				// 						: <int> -> <bool>								;
			};

#define INSTRUCTION_CASES(APPLY, APPLY_VALUE, APPLY_FUNCTION, APPLY_FUNCTION_COUNT, APPLY_VARIABLE, APPLY_COUNT, APPLY_FLAG_TYPEDESCRIPTOR, APPLY_PROPERTY, APPLY_METHOD_COUNT, APPLY_EVENT, APPLY_LABEL, APPLY_TYPE)\
			APPLY(Nop)\
			APPLY_VALUE(LoadValue)\
			APPLY_FUNCTION_COUNT(LoadClosure)\
			APPLY(LoadException)\
			APPLY_VARIABLE(LoadLocalVar)\
			APPLY_VARIABLE(LoadCapturedVar)\
			APPLY_VARIABLE(LoadGlobalVar)\
			APPLY_VARIABLE(StoreLocalVar)\
			APPLY_VARIABLE(StoreGlobalVar)\
			APPLY_COUNT(Duplicate)\
			APPLY(Pop)\
			APPLY(Return)\
			APPLY_COUNT(CreateArray)\
			APPLY_COUNT(CreateMap)\
			APPLY_COUNT(CreateInterface)\
			APPLY_TYPE(CreateRange)\
			APPLY(ReverseEnumerable)\
			APPLY(DeleteRawPtr)\
			APPLY_FLAG_TYPEDESCRIPTOR(ConvertToType)\
			APPLY_FLAG_TYPEDESCRIPTOR(TryConvertToType)\
			APPLY_FLAG_TYPEDESCRIPTOR(TestType)\
			APPLY(GetType)\
			APPLY_LABEL(Jump)\
			APPLY_LABEL(JumpIf)\
			APPLY_FUNCTION_COUNT(Invoke)\
			APPLY_PROPERTY(GetProperty)\
			APPLY_COUNT(InvokeProxy)\
			APPLY_METHOD_COUNT(InvokeMethod)\
			APPLY_EVENT(AttachEvent)\
			APPLY(DetachEvent)\
			APPLY_LABEL(InstallTry)\
			APPLY_COUNT(UninstallTry)\
			APPLY(RaiseException)\
			APPLY(TestElementInSet)\
			APPLY_TYPE(CompareLiteral)\
			APPLY(CompareStruct)\
			APPLY(CompareReference)\
			APPLY(CompareValue)\
			APPLY_TYPE(OpNot)\
			APPLY_TYPE(OpPositive)\
			APPLY_TYPE(OpNegative)\
			APPLY(OpConcat)\
			APPLY_TYPE(OpExp)\
			APPLY_TYPE(OpAdd)\
			APPLY_TYPE(OpSub)\
			APPLY_TYPE(OpMul)\
			APPLY_TYPE(OpDiv)\
			APPLY_TYPE(OpMod)\
			APPLY_TYPE(OpShl)\
			APPLY_TYPE(OpShr)\
			APPLY_TYPE(OpXor)\
			APPLY_TYPE(OpAnd)\
			APPLY_TYPE(OpOr)\
			APPLY(OpLT)\
			APPLY(OpGT)\
			APPLY(OpLE)\
			APPLY(OpGE)\
			APPLY(OpEQ)\
			APPLY(OpNE)\

			enum class WfInsType
			{
				Bool,
				I1,
				I2,
				I4,
				I8,
				U1,
				U2,
				U4,
				U8,
				F4,
				F8,
				String,
				Unknown,
			};

			struct WfInstruction
			{
				WfInsCode											code = WfInsCode::Nop;
				reflection::description::Value						valueParameter;
				vint												countParameter = 0;
				union
				{
					struct
					{
						reflection::description::Value::ValueType		flagParameter;
						reflection::description::ITypeDescriptor*		typeDescriptorParameter;
					};
					WfInsType											typeParameter;
					vint												indexParameter;
					reflection::description::IPropertyInfo*				propertyParameter;
					reflection::description::IMethodInfo*				methodParameter;
					reflection::description::IEventInfo*				eventParameter;
				};

				WfInstruction();

				#define CTOR(NAME)						static WfInstruction NAME();
				#define CTOR_VALUE(NAME)				static WfInstruction NAME(const reflection::description::Value& value);
				#define CTOR_FUNCTION(NAME)				static WfInstruction NAME(vint function);
				#define CTOR_FUNCTION_COUNT(NAME)		static WfInstruction NAME(vint function, vint count);
				#define CTOR_VARIABLE(NAME)				static WfInstruction NAME(vint variable);
				#define CTOR_COUNT(NAME)				static WfInstruction NAME(vint count);
				#define CTOR_FLAG_TYPEDESCRIPTOR(NAME)	static WfInstruction NAME(reflection::description::Value::ValueType flag, reflection::description::ITypeDescriptor* typeDescriptor);
				#define CTOR_PROPERTY(NAME)				static WfInstruction NAME(reflection::description::IPropertyInfo* propertyInfo);
				#define CTOR_METHOD_COUNT(NAME)			static WfInstruction NAME(reflection::description::IMethodInfo* methodInfo, vint count);
				#define CTOR_EVENT(NAME)				static WfInstruction NAME(reflection::description::IEventInfo* eventInfo);
				#define CTOR_LABEL(NAME)				static WfInstruction NAME(vint label);
				#define CTOR_TYPE(NAME)					static WfInstruction NAME(WfInsType type);

				INSTRUCTION_CASES(
					CTOR,
					CTOR_VALUE,
					CTOR_FUNCTION,
					CTOR_FUNCTION_COUNT,
					CTOR_VARIABLE,
					CTOR_COUNT,
					CTOR_FLAG_TYPEDESCRIPTOR,
					CTOR_PROPERTY,
					CTOR_METHOD_COUNT,
					CTOR_EVENT,
					CTOR_LABEL,
					CTOR_TYPE)

				#undef CTOR
				#undef CTOR_VALUE
				#undef CTOR_FUNCTION
				#undef CTOR_FUNCTION_COUNT
				#undef CTOR_VARIABLE
				#undef CTOR_COUNT
				#undef CTOR_FLAG_TYPEDESCRIPTOR
				#undef CTOR_PROPERTY
				#undef CTOR_METHOD_COUNT
				#undef CTOR_EVENT
				#undef CTOR_LABEL
				#undef CTOR_TYPE
			};

/***********************************************************************
Assembly
***********************************************************************/

			/// <summary>Representing a compiled function.</summary>
			class WfAssemblyFunction : public Object
			{
			public:
				/// <summary>Name of the function.</summary>
				WString												name;
				/// <summary>Argument names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.arguments] when debugging.</summary>
				collections::List<WString>							argumentNames;
				/// <summary>Captured variable names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.captured] when debugging.</summary>
				collections::List<WString>							capturedVariableNames;
				/// <summary>Local variable names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.localVariables] when debugging.</summary>
				collections::List<WString>							localVariableNames;
				/// <summary>First instruction index of the function. This index is for accessing [F:vl.workflow.runtime.WfAssembly.instructions].</summary>
				vint												firstInstruction = -1;
				/// <summary>Last instruction index of the function. This index is for accessing [F:vl.workflow.runtime.WfAssembly.instructions].</summary>
				vint												lastInstruction = -1;
			};

			/// <summary>Representing debug informations.</summary>
			class WfInstructionDebugInfo : public Object
			{
			public:

				/// <summary>Module codes.</summary>
				collections::List<WString>							moduleCodes;				// codeIndex -> code
				/// <summary>Mapping information from every instruction to code. Use[F:vl.parsing.ParsingTextRange.codeIndex] for accessing <see cref="moduleCodes"/>.</summary>
				collections::List<parsing::ParsingTextRange>		instructionCodeMapping;		// instruction -> range
				/// <summary>Mapping information from every row in codes to instructions. Value of the key is (codeIndex, row).</summary>
				collections::Group<Tuple<vint, vint>, vint>			codeInstructionMapping;		// (codeIndex, row) -> instruction [generated]

				void												Initialize();
			};

			/// <summary>Representing a Workflow assembly.</summary>
			class WfAssembly : public Object, public reflection::Description<WfAssembly>
			{
			protected:
				template<typename TIO>
				void IO(TIO& io);
			public:
				/// <summary>Debug informations using the module code.</summary>
				Ptr<WfInstructionDebugInfo>							insBeforeCodegen;
				/// <summary>Debug informations using the module code from generated syntax trees from the final compiling pass.</summary>
				Ptr<WfInstructionDebugInfo>							insAfterCodegen;
				/// <summary>Global variable names. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.global] when debugging.</summary>
				collections::List<WString>							variableNames;
				/// <summary>Mapping from function names to function indexes for accessing <see cref="functions"/>.</summary>
				collections::Group<WString, vint>					functionByName;
				/// <summary>Functions.</summary>
				collections::List<Ptr<WfAssemblyFunction>>			functions;
				/// <summary>Instructions.</summary>
				collections::List<WfInstruction>					instructions;

				/// <summary>Create an empty assembly.</summary>
				WfAssembly();
				/// <summary>Deserialize an assembly.</summary>
				/// <param name="input">Serialized binary data.</param>
				WfAssembly(stream::IStream& input);
				
				void												Initialize();
				/// <summary>Serialize an assembly.</summary>
				/// <param name="output">Serialized binary data.</param>
				void												Serialize(stream::IStream& output);
			};

/***********************************************************************
RuntimeEnvironment
***********************************************************************/

			class WfRuntimeVariableContext : public Object
			{
				typedef collections::Array<reflection::description::Value>		VariableArray;

			public:
				VariableArray					variables;
			};

			/// <summary>Global context for executing a Workflow program. After the context is prepared, use [M:vl.workflow.runtime.LoadFunction] to call any functions inside the assembly. Function "&lt;initialize&gt;" should be the first to execute.</summary>
			class WfRuntimeGlobalContext : public Object
			{
			public:
				Ptr<WfAssembly>					assembly;
				Ptr<WfRuntimeVariableContext>	globalVariables;
				
				/// <summary>Create a global context for executing a Workflow program.</summary>
				/// <param name="_assembly">The assembly.</param>
				WfRuntimeGlobalContext(Ptr<WfAssembly> _assembly);
			};

			struct WfRuntimeStackFrame
			{
				Ptr<WfRuntimeVariableContext>	capturedVariables;
				vint							functionIndex = -1;
				vint							nextInstructionIndex = -1;
				vint							stackBase = 0;
				vint							fixedVariableCount = 0;
				vint							freeStackBase = 0;
			};

			struct WfRuntimeTrapFrame
			{
				vint							stackFrameIndex = -1;
				vint							instructionIndex = -1;
				vint							stackPatternCount = -1;
			};

/***********************************************************************
RuntimeException
***********************************************************************/

			/// <summary>Representing a call stack item.</summary>
			class WfRuntimeCallStackInfo : public Object, public virtual reflection::description::IValueCallStack
			{
				using IValueReadonlyDictionary = reflection::description::IValueReadonlyDictionary;
			protected:
				Ptr<IValueReadonlyDictionary>	cachedLocalVariables;
				Ptr<IValueReadonlyDictionary>	cachedLocalArguments;
				Ptr<IValueReadonlyDictionary>	cachedCapturedVariables;
				Ptr<IValueReadonlyDictionary>	cachedGlobalVariables;

				Ptr<IValueReadonlyDictionary>	GetVariables(collections::List<WString>& names, Ptr<WfRuntimeVariableContext> context, Ptr<IValueReadonlyDictionary>& cache);
			public:
				WfRuntimeCallStackInfo();
				WfRuntimeCallStackInfo(WfRuntimeThreadContext* context, const WfRuntimeStackFrame& stackFrame);
				~WfRuntimeCallStackInfo();

				/// <summary>The executing assembly.</summary>
				Ptr<WfAssembly>					assembly;
				/// <summary>Global variable values.</summary>
				Ptr<WfRuntimeVariableContext>	global;
				/// <summary>Captured variable values.</summary>
				Ptr<WfRuntimeVariableContext>	captured;
				/// <summary>Argument values.</summary>
				Ptr<WfRuntimeVariableContext>	arguments;
				/// <summary>Local variable values.</summary>
				Ptr<WfRuntimeVariableContext>	localVariables;
				/// <summary>The executing function.</summary>
				vint							functionIndex = -1;
				/// <summary>The executing instruction.</summary>
				vint							instruction = -1;

				Ptr<IValueReadonlyDictionary>	GetLocalVariables()override;
				Ptr<IValueReadonlyDictionary>	GetLocalArguments()override;
				Ptr<IValueReadonlyDictionary>	GetCapturedVariables()override;
				Ptr<IValueReadonlyDictionary>	GetGlobalVariables()override;

				/// <summary>Get the name of the executing function.</summary>
				/// <returns>The name of the execution function.</returns>
				WString							GetFunctionName()override;
				/// <summary>Get the source code of the executing module.</summary>
				/// <returns>The source code.</returns>
				WString							GetSourceCodeBeforeCodegen()override;
				/// <summary>Get the source code of the executing module from generated syntax trees from the final compiling pass.</summary>
				/// <returns>The source code.</returns>
				WString							GetSourceCodeAfterCodegen()override;
				/// <summary>Get the row number (starts at 0) of the source code of the executing module.</summary>
				/// <returns>The row number.</returns>
				vint							GetRowBeforeCodegen()override;
				/// <summary>Get the row number (starts at 0) of the source code of the executing module from generated syntax trees from the final compiling pass.</summary>
				/// <returns>The row number.</returns>
				vint							GetRowAfterCodegen()override;
			};
			
			/// <summary>Representing an raised exception.</summary>
			class WfRuntimeExceptionInfo : public Object, public virtual reflection::description::IValueException
			{
				typedef collections::List<Ptr<WfRuntimeCallStackInfo>>		CallStackList;
				using IValueReadonlyList = reflection::description::IValueReadonlyList;
			protected:
				Ptr<IValueReadonlyList>			cachedCallStack;

			public:
				/// <summary>Exception message.</summary>
				WString							message;
				/// <summary>Fatal error flag.</summary>
				bool							fatal = false;
				/// <summary>All call stack items.</summary>
				CallStackList					callStack;

				WfRuntimeExceptionInfo(const WString& _message, bool _fatal);
				~WfRuntimeExceptionInfo();
				
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				WString							GetMessage()override;
#pragma pop_macro("GetMessage")
				bool							GetFatal()override;
				Ptr<IValueReadonlyList>			GetCallStack()override;
			};
			
			/// <summary>Representing an raised exception object for upper level C++ code.</summary>
			class WfRuntimeException : public reflection::description::TypeDescriptorException
			{
			protected:
				Ptr<WfRuntimeExceptionInfo>		info;
				bool							fatal = false;
			public:
				WfRuntimeException(Ptr<WfRuntimeExceptionInfo> _info)
					:reflection::description::TypeDescriptorException(_info->message)
					, info(_info)
					, fatal(_info->fatal)
				{
				}

				WfRuntimeException(const WString& _message, bool _fatal)
					:reflection::description::TypeDescriptorException(_message)
					, fatal(_fatal)
				{
				}

				/// <summary>Get the detailed information.</summary>
				/// <returns>The detailed information.</returns>
				Ptr<WfRuntimeExceptionInfo> GetInfo()const
				{
					return info;
				}

				/// <summary>Get the fatal error flag.</summary>
				/// <returns>Returns true if this exception is a fatal error, which normally means state corruption in a Workflow runtime.</returns>
				bool IsFatal()const
				{
					return fatal;
				}
			};

/***********************************************************************
RuntimeThreadContext
***********************************************************************/

			enum class WfRuntimeExecutionStatus
			{
				Ready,
				Executing,
				RaisedException,
				Finished,
				FatalError,
			};

			enum class WfRuntimeExecutionAction
			{
				ExecuteInstruction,
				UnwrapStack,
				EnterStackFrame,
				ExitStackFrame,
				Nop,
			};

			enum class WfRuntimeThreadContextError
			{
				Success,
				WrongStackItemIndex,
				WrongVariableIndex,
				WrongFunctionIndex,
				WrongArgumentCount,
				WrongCapturedVariableCount,
				EmptyStackFrame,
				EmptyTrapFrame,
				EmptyStack,
				TrapFrameCorrupted,
				StackCorrupted,
			};

			struct WfRuntimeThreadContext
			{
				typedef collections::List<reflection::description::Value>		VariableList;
				typedef collections::List<WfRuntimeStackFrame>					StackFrameList;
				typedef collections::List<WfRuntimeTrapFrame>					TrapFrameList;

				Ptr<WfRuntimeGlobalContext>		globalContext;
				Ptr<WfRuntimeExceptionInfo>		exceptionInfo;
				VariableList					stack;
				StackFrameList					stackFrames;
				TrapFrameList					trapFrames;
				WfRuntimeExecutionStatus		status = WfRuntimeExecutionStatus::Finished;

				WfRuntimeThreadContext(Ptr<WfRuntimeGlobalContext> _context);
				WfRuntimeThreadContext(Ptr<WfAssembly> _assembly);

				WfRuntimeStackFrame&			GetCurrentStackFrame();
				WfRuntimeThreadContextError		PushStackFrame(vint functionIndex, vint argumentCount, Ptr<WfRuntimeVariableContext> capturedVariables = 0);
				WfRuntimeThreadContextError		PopStackFrame();
				WfRuntimeTrapFrame&				GetCurrentTrapFrame();
				WfRuntimeThreadContextError		PushTrapFrame(vint instructionIndex);
				WfRuntimeThreadContextError		PopTrapFrame(vint saveStackPatternCount);
				WfRuntimeThreadContextError		PushValue(const reflection::description::Value& value);
				WfRuntimeThreadContextError		PopValue(reflection::description::Value& value);
				WfRuntimeThreadContextError		RaiseException(const WString& exception, bool fatalError, bool skipDebugger = false);
				WfRuntimeThreadContextError		RaiseException(Ptr<WfRuntimeExceptionInfo> info, bool skipDebugger = false);

				WfRuntimeThreadContextError		LoadStackValue(vint stackItemIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadGlobalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreGlobalVariable(vint variableIndex, const reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadCapturedVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadLocalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreLocalVariable(vint variableIndex, const reflection::description::Value& value);

				WfRuntimeExecutionAction		ExecuteInternal(WfInstruction& ins, WfRuntimeStackFrame& stackFrame, IWfDebuggerCallback* callback);
				WfRuntimeExecutionAction		Execute(IWfDebuggerCallback* callback);
				void							ExecuteToEnd();
			};

/***********************************************************************
Debugger
***********************************************************************/

			/// <summary>Break point action.</summary>
			class IWfBreakPointAction : public virtual Interface
			{
			public:
				/// <summary>Called a break point is about to activate.</summary>
				/// <returns>Returns false to skip this break point.</returns>
				/// <param name="debugger">The current attached debugger.</param>
				virtual bool					EvaluateCondition(WfDebugger* debugger) = 0;
				/// <summary>Called when a break point is about to activate, even <see cref="EvaluateCondition"/> returns false.</summary>
				/// <param name="debugger">The current attached debugger.</param>
				virtual void					PostAction(WfDebugger* debugger) = 0;
			};

			/// <summary>Break point.</summary>
			struct WfBreakPoint
			{
				enum Type
				{
					Instruction,	// assembly, instruction
					ReadGlobalVar,	// assembly, variable
					WriteGlobalVar,	// assembly, variable
					GetProperty,	// [thisObject], propertyInfo
					SetProperty,	// [thisObject], propertyInfo
					AttachEvent,	// [thisObject], eventInfo
					DetachEvent,	// [thisObject], eventInfo
					InvokeMethod,	// [thisObject], methodInfo
					CreateObject,	// typeDescriptor
				};

				vint											id = -1;
				bool											available = false;
				bool											enabled = false;
				Ptr<IWfBreakPointAction>						action;

				Type											type;
				WfAssembly*										assembly = nullptr;
				union
				{
					vint										instruction = -1;
					vint										variable;
				};

				reflection::DescriptableObject*					thisObject = nullptr;
				union
				{
					reflection::description::IPropertyInfo*		propertyInfo = nullptr;
					reflection::description::IEventInfo*		eventInfo;
					reflection::description::IMethodInfo*		methodInfo;
					reflection::description::ITypeDescriptor*	typeDescriptor;
				};

				/// <summary>Create an instruction break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="instruction">The index of the instruction.</param>
				static WfBreakPoint								Ins(WfAssembly* assembly, vint instruction);
				
				/// <summary>Create an global variable reading break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="variable">The index of the global variable.</param>
				static WfBreakPoint								Read(WfAssembly* assembly, vint variable);
				
				/// <summary>Create an global variable writing break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="variable">The index of the global variable.</param>
				static WfBreakPoint								Write(WfAssembly* assembly, vint variable);
				
				/// <summary>Create an property reading break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="propertyInfo">The property.</param>
				static WfBreakPoint								Get(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				
				/// <summary>Create an property writing break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="propertyInfo">The property.</param>
				static WfBreakPoint								Set(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				
				/// <summary>Create an event attaching break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="eventInfo">The event.</param>
				static WfBreakPoint								Attach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				
				/// <summary>Create an event detaching break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="eventInfo">The event.</param>
				static WfBreakPoint								Detach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				
				/// <summary>Create an function invoking break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="methodInfo">The function.</param>
				static WfBreakPoint								Invoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo);
				
				/// <summary>Create an object creating break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="typeDescriptor">The target object type.</param>
				static WfBreakPoint								Create(reflection::description::ITypeDescriptor* typeDescriptor);
			};

			class IWfDebuggerCallback : public virtual Interface
			{
			public:
				virtual void					EnterThreadContext(WfRuntimeThreadContext* context) = 0;
				virtual void					LeaveThreadContext(WfRuntimeThreadContext* context) = 0;
				virtual bool					BreakIns(WfAssembly* assembly, vint instruction) = 0;
				virtual bool					BreakRead(WfAssembly* assembly, vint variable) = 0;
				virtual bool					BreakWrite(WfAssembly* assembly, vint variable) = 0;
				virtual bool					BreakGet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo) = 0;
				virtual bool					BreakSet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo) = 0;
				virtual bool					BreakAttach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo) = 0;
				virtual bool					BreakDetach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo) = 0;
				virtual bool					BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo) = 0;
				virtual bool					BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor) = 0;
				virtual bool					BreakException(Ptr<WfRuntimeExceptionInfo> info) = 0;
				virtual bool					WaitForContinue() = 0;
			};

			/// <summary>Workflow debugger.</summary>
			class WfDebugger : public Object, protected virtual IWfDebuggerCallback
			{
				friend IWfDebuggerCallback* GetDebuggerCallback(WfDebugger* debugger);

				typedef collections::List<WfBreakPoint>					BreakPointList;
				typedef collections::List<WfRuntimeThreadContext*>		ThreadContextList;

				typedef Tuple<WfAssembly*, vint>														AssemblyKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IPropertyInfo*>	PropertyKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IEventInfo*>	EventKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IMethodInfo*>	MethodKey;
				typedef reflection::description::ITypeDescriptor*										TypeKey;

				typedef collections::Dictionary<AssemblyKey, vint>		AssemblyBreakPointMap;
				typedef collections::Dictionary<PropertyKey, vint>		PropertyBreakPointMap;
				typedef collections::Dictionary<EventKey, vint>			EventBreakPointMap;
				typedef collections::Dictionary<MethodKey, vint>		MethodBreakPointMap;
				typedef collections::Dictionary<TypeKey, vint>			TypeBreakPointMap;
			public:
				enum State
				{						//		Run		Pause	Stop	StepOver	StepInto
					Running,			// R			*RTP	*RTS
					PauseByOperation,	// PBO	*C				*RTS	*C			*C
					PauseByBreakPoint,	// PBB	*C				*RTS	*C			*C
					Stopped,			// S			*RTP			*			*
					Continue,			// C	soon becomes Running
					RequiredToPause,	// RTP	soon becomes PauseByOperation
					RequiredToStop,		// RTS	soon becomes Stop
				};

				enum RunningType
				{
					RunUntilBreakPoint,
					RunStepOver,
					RunStepInto,
				};

				struct InstructionLocation
				{
					vint								contextIndex = -1;
					WfAssembly*							assembly = nullptr;
					vint								stackFrameIndex = -1;
					vint								instruction = -1;

					bool								BreakStepOver(const InstructionLocation& il, bool beforeCodegen);
					bool								BreakStepInto(const InstructionLocation& il, bool beforeCodegen);
				};

				static const vint						InvalidBreakPoint = -1;
				static const vint						PauseBreakPoint = -2;
			protected:
				BreakPointList							breakPoints;
				collections::List<vint>					freeBreakPointIndices;
				volatile bool							evaluatingBreakPoint = false;
				volatile bool							breakException = false;

				ThreadContextList						threadContexts;

				volatile State							state = Stopped;
				volatile RunningType					runningType = RunUntilBreakPoint;
				volatile vint							lastActivatedBreakPoint = InvalidBreakPoint;
				bool									stepBeforeCodegen = true;
				InstructionLocation						instructionLocation;

				AssemblyBreakPointMap					insBreakPoints;
				AssemblyBreakPointMap					getGlobalVarBreakPoints;
				AssemblyBreakPointMap					setGlobalVarBreakPoints;
				PropertyBreakPointMap					getPropertyBreakPoints;
				PropertyBreakPointMap					setPropertyBreakPoints;
				EventBreakPointMap						attachEventBreakPoints;
				EventBreakPointMap						detachEventBreakPoints;
				MethodBreakPointMap						invokeMethodBreakPoints;
				TypeBreakPointMap						createObjectBreakPoints;

				/// <summary>Called for doing something when a break point is activated. This function will be called multiple times before some one let the debugger to continue.</summary>
				virtual void							OnBlockExecution();
				/// <summary>Called when a new Workflow program is about to run.</summary>
				virtual void							OnStartExecution();
				/// <summary>Called when a Workflow program is stopped by any reason.</summary>
				virtual void							OnStopExecution();
				
				InstructionLocation						MakeCurrentInstructionLocation();
				template<typename TKey>
				bool									HandleBreakPoint(const TKey& key, collections::Dictionary<TKey, vint>& breakPointMap);
				bool									SetBreakPoint(const WfBreakPoint& breakPoint, bool available, vint index);
				
				void									EnterThreadContext(WfRuntimeThreadContext* context)override;
				void									LeaveThreadContext(WfRuntimeThreadContext* context)override;
				bool									BreakIns(WfAssembly* assembly, vint instruction)override;
				bool									BreakRead(WfAssembly* assembly, vint variable)override;
				bool									BreakWrite(WfAssembly* assembly, vint variable)override;
				bool									BreakGet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)override;
				bool									BreakSet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)override;
				bool									BreakAttach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)override;
				bool									BreakDetach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)override;
				bool									BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo)override;
				bool									BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor)override;
				bool									BreakException(Ptr<WfRuntimeExceptionInfo> info)override;
				bool									WaitForContinue()override;
			public:
				/// <summary>Create a debugger.</summary>
				WfDebugger();
				~WfDebugger();

				/// <summary>Add a new break point.</summary>
				/// <returns>Returns the index of this break point. Returns -1 if failed.</returns>
				/// <param name="breakPoint">The break point.</param>
				vint									AddBreakPoint(const WfBreakPoint& breakPoint);
				/// <summary>Add a new source code break point.</summary>
				/// <returns>Returns the index of this break point. Returns -1 if failed.</returns>
				/// <param name="assembly">The assembly.</param>
				/// <param name="codeIndex">The code index of a module.</param>
				/// <param name="row">The row number, starts from 0.</param>
				/// <param name="beforeCodegen">Set to true to apply source code information to original source code.</param>
				vint									AddCodeLineBreakPoint(WfAssembly* assembly, vint codeIndex, vint row, bool beforeCodegen = true);
				/// <summary>Get the number of all break points.</summary>
				/// <returns>The number of all break points.</returns>
				vint									GetBreakPointCount();
				/// <summary>Get a specified break point.</summary>
				/// <returns>The break point.</returns>
				/// <param name="index">The index of the break point.</param>
				const WfBreakPoint&						GetBreakPoint(vint index);
				/// <summary>Delete a specified break point.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="index">The index of the break point.</param>
				bool									RemoveBreakPoint(vint index);
				/// <summary>Enable or disable a specified break point.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="index">The index of the break point.</param>
				/// <param name="enabled">Set to true to enable a break point.</param>
				bool									EnableBreakPoint(vint index, bool enabled);
				/// <summary>Test if the exception break point is enabled or not.</summary>
				/// <returns>Returns true if the exception break point is enabled.</returns>
				bool									GetBreakException();
				/// <summary>Enable or disable the exception break point.</summary>
				/// <param name="value">Set to true to enable the exception break point.</param>
				void									SetBreakException(bool value);

				/// <summary>Continue to run the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Run();
				/// <summary>Pause the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Pause();
				/// <summary>Stop the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Stop();
				/// <summary>Run until reached the next row in the same function or outside of this function.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				bool									StepOver(bool beforeCodegen = true);
				/// <summary>Run until reached the next row.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				bool									StepInto(bool beforeCodegen = true);
				/// <summary>Get the current state of the debugger.</summary>
				/// <returns>The state of the debugger.</returns>
				State									GetState();
				/// <summary>Get the running type of the debugger.</summary>
				/// <returns>The running type of the debugger.</returns>
				RunningType								GetRunningType();
				/// <summary>Get the index of the last activated break point.</summary>
				/// <returns>The index of the last activated break point.</returns>
				vint									GetLastActivatedBreakPoint();

				const ThreadContextList&				GetThreadContexts();
				WfRuntimeThreadContext*					GetCurrentThreadContext();
				/// <summary>Get the current position of the executing instruction in the source code.</summary>
				/// <returns>The current position in the source code.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				/// <param name="context">Specify a thread context. Set to null to use the top thread context.</param>
				/// <param name="callStackIndex">Specify a call stack index. Set to null to use the top call stack item of the specified thread context.</param>
				const parsing::ParsingTextRange&		GetCurrentPosition(bool beforeCodegen = true, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
				/// <summary>Get the variable value by name in the current scope.</summary>
				/// <returns>The value.</returns>
				/// <param name="name">The name.</param>
				/// <param name="context">Specify a thread context. Set to null to use the top thread context.</param>
				/// <param name="callStackIndex">Specify a call stack index. Set to null to use the top call stack item of the specified thread context.</param>
				reflection::description::Value			GetValueByName(const WString& name, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
			};

			extern IWfDebuggerCallback*					GetDebuggerCallback();
			extern IWfDebuggerCallback*					GetDebuggerCallback(WfDebugger* debugger);
			
			/// <summary>Get the debugger for the current thread.</summary>
			/// <returns>The debugger.</returns>
			extern Ptr<WfDebugger>						GetDebuggerForCurrentThread();
			/// <summary>Set the debugger for the current thread.</summary>
			/// <param name="debugger">The debugger.</param>
			extern void									SetDebugferForCurrentThread(Ptr<WfDebugger> debugger);

/***********************************************************************
Helper Functions
***********************************************************************/
			
			/// <summary>Load a function from a global context, raise an exception if multiple functions are found under the same name. Function "&gt;initialize&lt;" should be the first to execute.</summary>
			/// <returns>The loaded function.</returns>
			/// <param name="context">The context to the evaluation environment.</param>
			/// <param name="name">The function name.</param>
			extern Ptr<reflection::description::IValueFunctionProxy>		LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name);
			
			/// <summary>Load a C++ friendly function from a global context, raise an exception if multiple functions are found under the same name. Function "&gt;initialize&lt;" should be the first to execute.</summary>
			/// <typeparam name="TFunction">Type of the function.</typeparam>
			/// <returns>The loaded C++ friendly function.</returns>
			/// <param name="context">The context to the evaluation environment.</param>
			/// <param name="name">The function name.</param>
			template<typename TFunction>
			Func<TFunction> LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name)
			{
				auto proxy = LoadFunction(context, name);
				Func<TFunction> function;
				reflection::description::UnboxParameter<Func<TFunction>>(reflection::description::Value::From(proxy), function);
				return function;
			}
		}
	}
}

#endif

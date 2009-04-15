// Use this file to explicitly instantiate templates
//
// To use this file for a class:
//
//   #define JTemplateType ____				<-- normally done by user's code
//   #define JTemplateName ____				<-- normally done by .tmpl file
//   #include <instantiate_template.h>		<-- normally done by .tmpl file
//
// To use this file for a function:
//
//   #define JTemplateType       ____		<-- normally done by user's code
//   #define JTemplateReturnType ____		<-- normally done by user's code
//   #define JTemplateArgList    ____		<-- normally done by user's code
//   #define JTemplateName       ____		<-- normally done by .tmpl file
//   #include <instantiate_template.h>		<-- normally done by .tmpl file

// instantiate function

#if defined JTemplateType && defined JTemplateName && \
	defined JTemplateReturnType && defined JTemplateArgList

	template JTemplateReturnType JTemplateName(JTemplateArgList);

// instantiate class

#elif defined JTemplateType && defined JTemplateName

	template class JTemplateName < JTemplateType >;

#endif

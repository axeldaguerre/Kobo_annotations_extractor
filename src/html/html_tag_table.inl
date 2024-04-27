/*
   NOTE: - The table is designed to allow you to include it multiple times to "pull out" other things
          from the table, such as html tagname as strings or enums, if a tag has a closing tag etc.
         - The order is used for priority, id est if we want a tag being a title, we will get the closer 
            from the top and ignore the ones below.
   
*/
#ifndef STRLEN
  #define STRLEN(X) (U64)(sizeof(X) - 1)
#endif
#ifndef STR
  #define STR(x) #x
#endif

#ifndef STR_WITH_TAGS_WITH_CLOSING
  #define STR_WITH_TAGS_WITH_CLOSING(x) STR(<##x></##x>)
#endif

#ifndef STR_WITH_SINGLE_TAG
  #define STR_WITH_SINGLE_TAG(x) STR(<##x>)
#endif

#ifndef STR_WITH_TAGS
  #define STR_WITH_TAGS(x, isSingle)((isSingle)?\
       STR_WITH_SINGLE_TAG(x),STRLEN(STR_WITH_SINGLE_TAG(x)):\
       STR_WITH_TAGS_WITH_CLOSING(x),STRLEN(STR_WITH_TAGS_WITH_CLOSING(x)))
#endif    


#ifndef TAG
  #define TAG(Tag, ClosingType, str, TextualTypes, ...) { HTMLTag_##Tag, HTMLTagEnclosingType_##ClosingType, str, TextualTypes, __VA_ARGS__ },
#endif

#ifndef true
  #define true 1
#endif
#ifndef false
  #define false 0
#endif

#define A          { (U8*)"a",         sizeof("a")-1 }
#define P          { (U8*)"p",         sizeof("p")-1 }
#define LI         { (U8*)"li",        sizeof("li")-1 }
#define UL         { (U8*)"ul",        sizeof("ul")-1 }
#define OL         { (U8*)"ol",        sizeof("ol")-1 }
#define H1         { (U8*)"h1",        sizeof("h1")-1 }
#define H2         { (U8*)"h2",        sizeof("h2")-1 }
#define H3         { (U8*)"h3",        sizeof("h3")-1 }
#define H4         { (U8*)"h4",        sizeof("h4")-1 }
#define DIV        { (U8*)"div",       sizeof("div")-1 }
#define IMG        { (U8*)"img",       sizeof("img")-1 }
#define BR         { (U8*)"br",        sizeof("br")-1 }
#define STRONG     { (U8*)"strong",    sizeof("strong")-1 }
#define SPAN       { (U8*)"span",      sizeof("span")-1 }
#define PRE        { (U8*)"pre",       sizeof("pre")-1 }
#define U          { (U8*)"u",         sizeof("u")-1 }
#define EM         { (U8*)"em",        sizeof("em")-1 }
#define CODE       { (U8*)"code",      sizeof("code")-1 }
#define SECTION    { (U8*)"section",   sizeof("section")-1 }
#define META       { (U8*)"meta",      sizeof("meta")-1 }
#define LINK       { (U8*)"link",      sizeof("link")-1 }
#define SCRIPT     { (U8*)"script",    sizeof("script")-1 }
#define SMALL      { (U8*)"small",     sizeof("small")-1 }
#define ARTICLE    { (U8*)"article",   sizeof("article")-1 }
#define NAV        { (U8*)"nav",       sizeof("nav")-1 }
#define HEADER     { (U8*)"header",    sizeof("header")-1 }
#define MAIN       { (U8*)"main",      sizeof("main")-1 }
#define ASIDE      { (U8*)"aside",     sizeof("aside")-1 }
#define TABLE      { (U8*)"table",     sizeof("table")-1 }
#define TR         { (U8*)"tr",        sizeof("tr")-1 }
#define TH         { (U8*)"th",        sizeof("th")-1 }
#define TD         { (U8*)"td",        sizeof("td")-1 }
#define FORM       { (U8*)"form",      sizeof("form")-1 }
#define INPUT      { (U8*)"input",     sizeof("input")-1 }
#define BUTTON     { (U8*)"button",    sizeof("button")-1 }
#define HEAD       { (U8*)"head",      sizeof("head")-1 }
#define DOCTYPE    { (U8*)"!DOCTYPE",  sizeof("!DOCTYPE")-1 }
#define HTML       { (U8*)"html",      sizeof("html")-1 }
#define TITLE      { (U8*)"title",     sizeof("title")-1 }
#define BODY       { (U8*)"body",      sizeof("body")-1 }
#define FOOTER     { (U8*)"footer",    sizeof("footer")-1 }
#define LABEL      { (U8*)"label",     sizeof("label")-1 }


TAG(a,      Paired,   A,       { TextType_Link })
TAG(p,      Paired,   P,       { TextType_Paragraph })
TAG(li,     Paired,   LI,      { TextType_Item_List })
TAG(ul,     Paired,   UL,      { TextType_Parent_List })
TAG(ol,     Paired,   OL,      { TextType_Item_List })
TAG(h1,     Paired,   H1,      { TextType_Title })
TAG(h2,     Paired,   H2,      { TextType_Heading1 })
TAG(h3,     Paired,   H3,      { TextType_Heading2 })
TAG(h4,     Paired,   H4,      { TextType_Heading3 })
TAG(div,    Paired,   DIV,     { TextType_Structural })
TAG(img,    Unique,   IMG,     { TextType_Visual })
TAG(br,     Unique,   BR,      { TextType_Esthetic })
TAG(strong, Paired,   STRONG,  { TextType_Esthetic })
TAG(span,   Paired,   SPAN,    { TextType_Esthetic })
TAG(pre,    Paired,   PRE,     { TextType_Esthetic })
TAG(u,      Paired,   U,       { TextType_Esthetic })
TAG(em,     Paired,   EM,      { TextType_Esthetic })
TAG(code,   Paired,   CODE,    { TextType_Code  | TextType_Date})
TAG(section,Paired,   SECTION, { TextType_Structural })
TAG(meta,   Unique,   META,    { TextType_Structural })
TAG(link,   Unique,   LINK,    { TextType_Structural })
TAG(script, Paired,   SCRIPT,  { TextType_Structural })
TAG(small,  Paired,   SMALL,   { TextType_Esthetic   })
TAG(article,Paired,   ARTICLE, { TextType_Structural })
TAG(nav,    Paired,   NAV,     { TextType_Structural })
TAG(header, Paired,   HEADER,  { TextType_Structural })
TAG(main,   Paired,   MAIN,    { TextType_Structural })
TAG(aside,  Paired,   ASIDE,   { TextType_Structural })
TAG(table,  Paired,   TABLE,   { TextType_Structural })
TAG(tr,     Paired,   TR,      { TextType_Structural })
TAG(th,     Paired,   TH,      { TextType_Structural })
TAG(td,     Paired,   TD,      { TextType_Structural })
TAG(form,   Paired,   FORM,    { TextType_Structural })
TAG(input,  Unique,   INPUT,   { TextType_Structural })
TAG(button, Paired,   BUTTON,  { TextType_Structural })
TAG(label,  Paired,   LABEL,   { TextType_Structural })
TAG(head,   Paired,   HEAD,    { TextType_Structural })
TAG(doctype,Unique,   DOCTYPE, { TextType_Structural })
TAG(html,   Paired,   HTML,    { TextType_Unmeaningfull })
TAG(title,  Paired,   TITLE,   { TextType_Structural })
TAG(body,   Paired,   BODY,    { TextType_Structural })
TAG(footer, Paired,   FOOTER,  { TextType_Structural })

#undef A
#undef P
#undef LI
#undef UL
#undef OL
#undef H1
#undef H2
#undef H3
#undef H4
#undef DIV
#undef IMG
#undef BR
#undef STRONG
#undef SPAN
#undef PRE
#undef U
#undef EM
#undef CODE
#undef SECTION
#undef META
#undef LINK
#undef SCRIPT
#undef SMALL
#undef ARTICLE
#undef NAV
#undef HEADER
#undef MAIN
#undef ASIDE
#undef TABLE
#undef TR
#undef TH
#undef TD
#undef FORM
#undef INPUT
#undef BUTTON
#undef LABEL
#undef HEAD
#undef DOCTYPE
#undef HTML
#undef TITLE
#undef BODY
#undef FOOTER

#undef TAG
/*
   NOTE: - The table is designed to allow you to include it multiple times to "pull out" other things
          from the table, such as html tagname as strings or enums, if a tag has a closing tag etc.
         - The order is used for priority, id est if we want a tag being a title, we will get the closer 
            from the top and ignore the ones below.
   
*/

#ifndef TAG
  #define TAG(TagType, ClosingType, str, TextualTypes) { TagType, HTMLTagEnclosingType_##ClosingType, str, TextualTypes},
#endif

#define HTMLTagType_NULL     0
#define HTMLTagType_A       (1ull << 1)
#define HTMLTagType_P       (1ull << 2)
#define HTMLTagType_LI      (1ull << 3)
#define HTMLTagType_UL      (1ull << 4)
#define HTMLTagType_OL      (1ull << 5)
#define HTMLTagType_H1      (1ull << 6)
#define HTMLTagType_H2      (1ull << 7)
#define HTMLTagType_H3      (1ull << 8)
#define HTMLTagType_H4      (1ull << 9)
#define HTMLTagType_DIV     (1ull << 10)
#define HTMLTagType_IMG     (1ull << 11)
#define HTMLTagType_BR      (1ull << 12)
#define HTMLTagType_STRONG  (1ull << 13)
#define HTMLTagType_SPAN    (1ull << 14)
#define HTMLTagType_PRE     (1ull << 15)
#define HTMLTagType_U       (1ull << 16)
#define HTMLTagType_EM      (1ull << 17)
#define HTMLTagType_CODE    (1ull << 18)
#define HTMLTagType_SECTION (1ull << 19)
#define HTMLTagType_META    (1ull << 20)
#define HTMLTagType_LINK    (1ull << 21)
#define HTMLTagType_SCRIPT  (1ull << 22)
#define HTMLTagType_SMALL   (1ull << 23)
#define HTMLTagType_ARTICLE (1ull << 24)
#define HTMLTagType_NAV     (1ull << 25)
#define HTMLTagType_HEADER  (1ull << 26)
#define HTMLTagType_MAIN    (1ull << 27)
#define HTMLTagType_ASIDE   (1ull << 28)
#define HTMLTagType_TABLE   (1ull << 29)
#define HTMLTagType_TR      (1ull << 31)
#define HTMLTagType_TH      (1ull << 32)
#define HTMLTagType_TD      (1ull << 33)
#define HTMLTagType_FORM    (1ull << 34)
#define HTMLTagType_INPUT   (1ull << 35)
#define HTMLTagType_BUTTON  (1ull << 36)
#define HTMLTagType_LABEL   (1ull << 43)
#define HTMLTagType_HEAD    (1ull << 37)
#define HTMLTagType_DOCTYPE (1ull << 38)
#define HTMLTagType_HTML    (1ull << 39)
#define HTMLTagType_TITLE   (1ull << 40)
#define HTMLTagType_BODY    (1ull << 41)
#define HTMLTagType_FOOTER  (1ull << 42)

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

TAG( HTMLTagType_A,       Paired,   A,       { TextType_Link })
TAG( HTMLTagType_P,       Paired,   P,       { TextType_Paragraph })
TAG( HTMLTagType_LI,      Paired,   LI,      { TextType_Item_List })
TAG( HTMLTagType_UL,      Paired,   UL,      { TextType_Parent_List })
TAG( HTMLTagType_OL,      Paired,   OL,      { TextType_Item_List })
TAG( HTMLTagType_H1,      Paired,   H1,      { TextType_Title })
TAG( HTMLTagType_H2,      Paired,   H2,      { TextType_Heading1 })
TAG( HTMLTagType_H3,      Paired,   H3,      { TextType_Heading2 })
TAG( HTMLTagType_H4,      Paired,   H4,      { TextType_Heading3 })
TAG( HTMLTagType_DIV,     Paired,   DIV,     { TextType_Structural })
TAG( HTMLTagType_IMG,     Unique,   IMG,     { TextType_Visual })
TAG( HTMLTagType_BR,      Unique,   BR,      { TextType_Esthetic })
TAG( HTMLTagType_STRONG,  Paired,   STRONG,  { TextType_Esthetic })
TAG( HTMLTagType_SPAN,    Paired,   SPAN,    { TextType_Esthetic })
TAG( HTMLTagType_PRE,     Paired,   PRE,     { TextType_Esthetic })
TAG( HTMLTagType_U,       Paired,   U,       { TextType_Esthetic })
TAG( HTMLTagType_EM,      Paired,   EM,      { TextType_Esthetic })
TAG( HTMLTagType_CODE,    Paired,   CODE,    { TextType_Code|TextType_Date})
TAG( HTMLTagType_SECTION, Paired,   SECTION, { TextType_Structural })
TAG( HTMLTagType_META,    Unique,   META,    { TextType_Structural })
TAG( HTMLTagType_LINK,    Unique,   LINK,    { TextType_Structural })
TAG( HTMLTagType_SCRIPT,  Paired,   SCRIPT,  { TextType_Structural })
TAG( HTMLTagType_SMALL,   Paired,   SMALL,   { TextType_Esthetic   })
TAG( HTMLTagType_ARTICLE, Paired,   ARTICLE, { TextType_Structural })
TAG( HTMLTagType_NAV,     Paired,   NAV,     { TextType_Structural })
TAG( HTMLTagType_HEADER,  Paired,   HEADER,  { TextType_Structural })
TAG( HTMLTagType_MAIN,    Paired,   MAIN,    { TextType_Structural })
TAG( HTMLTagType_ASIDE,   Paired,   ASIDE,   { TextType_Structural })
TAG( HTMLTagType_TABLE,   Paired,   TABLE,   { TextType_Structural })
TAG( HTMLTagType_TR,      Paired,   TR,      { TextType_Structural })
TAG( HTMLTagType_TH,      Paired,   TH,      { TextType_Structural })
TAG( HTMLTagType_TD,      Paired,   TD,      { TextType_Structural })
TAG( HTMLTagType_FORM,    Paired,   FORM,    { TextType_Structural })
TAG( HTMLTagType_INPUT,   Unique,   INPUT,   { TextType_Structural })
TAG( HTMLTagType_BUTTON,  Paired,   BUTTON,  { TextType_Structural })
TAG( HTMLTagType_LABEL,   Paired,   LABEL,   { TextType_Structural })
TAG( HTMLTagType_HEAD,    Paired,   HEAD,    { TextType_Structural })
TAG( HTMLTagType_DOCTYPE, Unique,   DOCTYPE, { TextType_Structural })
TAG( HTMLTagType_HTML,    Paired,   HTML,    { TextType_Unmeaningfull })
TAG( HTMLTagType_TITLE,   Paired,   TITLE,   { TextType_Structural })
TAG( HTMLTagType_BODY,    Paired,   BODY,    { TextType_Structural })
TAG( HTMLTagType_FOOTER,  Paired,   FOOTER,  { TextType_Structural })

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
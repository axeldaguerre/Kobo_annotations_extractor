/*
   NOTE: - The table is designed to allow you to include it multiple times to "pull out" other things
          from the table, such as html tagname as strings or enums, if a tag has a closing tag etc.
         - The order is used for priority, id est if we want a tag being a title, we will get the closer 
            from the top and ignore the ones below.
*/
/*
  TODO: Update is very laborious, when implementation will be done there will be very few updates.
        but still it can be way more convenient:
        - We should not need to have flags for HTMLTAG_
        - If we do spread inside a struct like before with include do we need to UNDEF all tags ?
        - HTMLTagContentType should be flags, like button is interactive and phrasing
*/
#ifndef TAG
  #define TAG(Tag, ContentType, FlowType, ClosingType, str, Strenght, RawMeaning)\
            { Tag, HTMLTagContentType_##ContentType, HTMLTagFlowContentType_##FlowType, HTMLTagEnclosingType_##ClosingType, str, RawStrenght_##Strenght, RawSemantic_##RawMeaning },
#endif
  
#define HTMLTag_NULL       0
#define HTMLTag_A          (1ull << 1)
#define HTMLTag_P          (1ull << 2)
#define HTMLTag_LI         (1ull << 3)
#define HTMLTag_UL         (1ull << 4)
#define HTMLTag_OL         (1ull << 5)
#define HTMLTag_H1         (1ull << 6)
#define HTMLTag_H2         (1ull << 7)
#define HTMLTag_H3         (1ull << 8)
#define HTMLTag_H4         (1ull << 9)
#define HTMLTag_H5         (1ull << 10)
#define HTMLTag_H6         (1ull << 11)
#define HTMLTag_DIV        (1ull << 12)
#define HTMLTag_IMG        (1ull << 13)
#define HTMLTag_I          (1ull << 14)
#define HTMLTag_BR         (1ull << 15)
#define HTMLTag_B          (1ull << 16)
#define HTMLTag_STRONG     (1ull << 17)
#define HTMLTag_SPAN       (1ull << 18)
#define HTMLTag_PRE        (1ull << 19)
#define HTMLTag_U          (1ull << 20)
#define HTMLTag_EM         (1ull << 21)
#define HTMLTag_SVG        (1ull << 22)
#define HTMLTag_CODE       (1ull << 23)
#define HTMLTag_SECTION    (1ull << 24)
#define HTMLTag_TEXTAREA   (1ull << 25)
#define HTMLTag_BLOCKQUOTE (1ull << 26)
#define HTMLTag_META       (1ull << 27)
#define HTMLTag_LINK       (1ull << 28)
#define HTMLTag_SCRIPT     (1ull << 29)
#define HTMLTag_SMALL      (1ull << 30)
#define HTMLTag_ARTICLE    (1ull << 31)
#define HTMLTag_NAV        (1ull << 32)
#define HTMLTag_HEADER     (1ull << 33)
#define HTMLTag_MAIN       (1ull << 34)
#define HTMLTag_ASIDE      (1ull << 35)
#define HTMLTag_TABLE      (1ull << 36)
#define HTMLTag_TR         (1ull << 37)
#define HTMLTag_TH         (1ull << 38)
#define HTMLTag_TD         (1ull << 39)
#define HTMLTag_FORM       (1ull << 40)
#define HTMLTag_INPUT      (1ull << 41)
#define HTMLTag_BUTTON     (1ull << 42)
#define HTMLTag_LABEL      (1ull << 43)
#define HTMLTag_HEAD       (1ull << 44)
#define HTMLTag_DOCTYPE    (1ull << 45)
#define HTMLTag_HTML       (1ull << 46)
#define HTMLTag_TITLE      (1ull << 47)
#define HTMLTag_BODY       (1ull << 48)
#define HTMLTag_FOOTER     (1ull << 49)

#define A          { (U8*)"a",         sizeof("a") -1 }
#define P          { (U8*)"p",         sizeof("p") -1 }
#define LI         { (U8*)"li",        sizeof("li")-1 }
#define UL         { (U8*)"ul",        sizeof("ul")-1 }
#define OL         { (U8*)"ol",        sizeof("ol")-1 }
#define H1         { (U8*)"h1",        sizeof("h1")-1 }
#define H2         { (U8*)"h2",        sizeof("h2")-1 }
#define H3         { (U8*)"h3",        sizeof("h3")-1 }
#define H4         { (U8*)"h4",        sizeof("h4")-1 }
#define H5         { (U8*)"h5",        sizeof("h5")-1 }
#define H6         { (U8*)"h6",        sizeof("h6")-1 }
#define DIV        { (U8*)"div",       sizeof("div")-1 }
#define IMG        { (U8*)"img",       sizeof("img")-1 }
#define I          { (U8*)"i",         sizeof("i")-1 }
#define BR         { (U8*)"br",        sizeof("br")-1 }
#define B          { (U8*)"b",         sizeof("b")-1 }
#define STRONG     { (U8*)"strong",    sizeof("strong")-1 }
#define SPAN       { (U8*)"span",      sizeof("span")-1 }
#define PRE        { (U8*)"pre",       sizeof("pre")-1 }
#define U          { (U8*)"u",         sizeof("u")-1 }
#define EM         { (U8*)"em",        sizeof("em")-1 }
#define CODE       { (U8*)"code",      sizeof("code")-1 }
#define SVG        { (U8*)"svg",      sizeof("svg")-1 }
#define SECTION    { (U8*)"section",   sizeof("section")-1 }
#define TEXTAREA   { (U8*)"textarea",   sizeof("textarea")-1 }
#define BLOCKQUOTE { (U8*)"blockquote",   sizeof("blockquote")-1 }
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

TAG( HTMLTag_A,          Phrasing,  Inline, Paired, A,          AboveMedium, Link)
TAG( HTMLTag_P,          Flow,      Block,  Paired, P,          AboveMedium, Details)
TAG( HTMLTag_LI,         Flow,      Block,  Paired, LI,         AboveMedium, Step)
TAG( HTMLTag_UL,         Flow,      Block,  Paired, UL,         Lowest,      Step)
TAG( HTMLTag_OL,         Flow,      Block,  Paired, OL,         Lowest,      Step)
TAG( HTMLTag_H1,         Flow,      Block,  Paired, H1,         Highest,     Summary)
TAG( HTMLTag_H2,         Flow,      Block,  Paired, H2,         High,        Summary)
TAG( HTMLTag_H3,         Flow,      Block,  Paired, H3,         AboveMedium, Summary)
TAG( HTMLTag_H4,         Flow,      Block,  Paired, H4,         Medium,      Summary)
TAG( HTMLTag_H5,         Flow,      Block,  Paired, H5,         Medium,      Summary)
TAG( HTMLTag_H6,         Flow,      Block,  Paired, H6,         Medium,      Summary)
TAG( HTMLTag_DIV,        Flow,      Block,  Paired, DIV,        Null,        Null)  
TAG( HTMLTag_IMG,        Embedded,  Inline, Unique, IMG,        AboveMedium, Visual)
TAG( HTMLTag_I,          Phrasing,  Inline, Unique, I,          Null,        Null) 
TAG( HTMLTag_BR,         Phrasing,  Inline, Unique, BR,         Null,        Null) 
TAG( HTMLTag_B,          Phrasing,  Inline, Unique, B,          Null,        Null) 
TAG( HTMLTag_STRONG,     Phrasing,  Inline, Paired, STRONG,     Null,        Null)
TAG( HTMLTag_SPAN,       Phrasing,  Inline, Paired, SPAN,       Null,        Null)
TAG( HTMLTag_PRE,        Flow,      Block,  Paired, PRE,        AboveMedium, Details)
TAG( HTMLTag_U,          Phrasing,  Inline, Paired, U,          Null,        Null)
TAG( HTMLTag_EM,         Phrasing,  Inline, Paired, EM,         Null,        Null)
TAG( HTMLTag_CODE,       Phrasing,  Inline, Paired, CODE,       AboveMedium, Details)
TAG( HTMLTag_SVG,        Embedded,  Inline, Paired, SVG,        AboveMedium, Details)
TAG( HTMLTag_SECTION,    Flow,      Block,  Paired, SECTION,    Null,        Null)
TAG( HTMLTag_TEXTAREA,   Phrasing,  Inline, Paired, TEXTAREA,   Null,        Null)
TAG( HTMLTag_BLOCKQUOTE, Flow,      Block,  Paired, BLOCKQUOTE, Null,        Null)
TAG( HTMLTag_META,       Phrasing,  Inline, Unique, META,       Null,        Null)
TAG( HTMLTag_LINK,       Phrasing,  Inline, Unique, LINK,       AboveMedium, Link)
TAG( HTMLTag_SCRIPT,     Scripting, Inline, Paired, SCRIPT,     Null,        Null)
TAG( HTMLTag_SMALL,      Phrasing,  Inline, Paired, SMALL,      Null,        Null)
TAG( HTMLTag_ARTICLE,    Flow,      Block,  Paired, ARTICLE,    Null,        Null)
TAG( HTMLTag_NAV,        Flow,      Block,  Paired, NAV,        Null,        Null)
TAG( HTMLTag_HEADER,     Flow,      Block,  Paired, HEADER,     Low,         Summary)
TAG( HTMLTag_MAIN,       Flow,      Block,  Paired, MAIN,       Null,        Null)
TAG( HTMLTag_ASIDE,      Flow,      Block,  Paired, ASIDE,      AboveMedium, Details)
TAG( HTMLTag_TABLE,      Flow,      Block,  Paired, TABLE,      Null,        Null)
TAG( HTMLTag_TR,         Flow,      Block,  Paired, TR,         Null,        Null)
TAG( HTMLTag_TH,         Flow,      Block,  Paired, TH,         Null,        Null)
TAG( HTMLTag_TD,         Flow,      Block,  Paired, TD,         Null,        Null)
TAG( HTMLTag_FORM,       Flow,      Block,  Paired, FORM,       Null,        Null)
TAG( HTMLTag_INPUT,      Phrasing,  Inline, Unique, INPUT,      Null,        Null)
TAG( HTMLTag_BUTTON,     Phrasing,  Inline, Paired, BUTTON,     Null,        Null)
TAG( HTMLTag_LABEL,      Phrasing,  Inline, Paired, LABEL,      Null,        Null)
TAG( HTMLTag_HEAD,       Metadata,  Block,  Paired, HEAD,       Null,        Null)
TAG( HTMLTag_DOCTYPE,    Document,  Block,  Unique, DOCTYPE,    Null,        Null)
TAG( HTMLTag_HTML,       Sectioning,Block,  Paired, HTML,       Null,        Null)
TAG( HTMLTag_TITLE,      Metadata,  Inline, Paired, TITLE,      Highest,     Summary)
TAG( HTMLTag_BODY,       Sectioning,Block,  Paired, BODY,       Null,        Null)
TAG( HTMLTag_FOOTER,     Flow,      Block,  Paired, FOOTER,     Null,        Null)

#undef A
#undef P
#undef I
#undef UL
#undef OL
#undef H1
#undef H2
#undef H3
#undef H4
#undef H5
#undef H6
#undef DIV
#undef IMG
#undef I
#undef BR
#undef B
#undef STRONG
#undef SPAN
#undef PRE
#undef U
#undef EM
#undef CODE
#undef SVG
#undef SECTION
#undef BLOCKQUOTE
#undef TEXTAREA
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

/*NOTE: ---------- Phrasing content
<a>
<abbr>
<area> (if it is a descendant of a <map> element)
<audio>
<b>
<bdi>
<bdo>
<br>
<button>
<canvas>
<cite>
<code>
<data>
<datalist>
<dfn>
<em>
<embed>
<i>
<iframe>
<img>
<input>
<kbd>
<label>
<link> (if it is a descendant of a <noscript> element or it has an itemprop attribute)
<map> (if it is a descendant of a <noscript> element)
<mark>
<math>
<meta> (if it is a descendant of a <noscript> element)
<meter>
<noscript> (if it is a descendant of a <noscript> element)
<object>
<output>
<picture>
<progress>
<q>
<ruby>
<s>
<samp>
<script>
<select>
<small>
<span>
<strong>
<sub>
<sup>
<svg>
<template>
<textarea>
<time>
<u>
<var>
<video>
<wbr>
*/
/*NOTE: ---------- Flow content
<address>
<article>
<aside>
<blockquote>
<details>
<dialog>
<div>
<dl>
<fieldset>
<figure>
<footer>
<form>
<header>
<h1>
<h2>
<h3>
<h4>
<h5>
<h6>
<hr>
<li>
<main>
<nav>
<ol>
<p>
<pre>
<section>
<table>
<ul>

*/ 
/*NOTE: ---------- Section content
<article>
<aside>
<nav>
<section>
*/

/*NOTE: ---------- Embedded content
<audio>
<canvas>
<embed>
<iframe>
<img>
<math>
<object>
<picture>
<svg>
<video>

*/
/*NOTE: ---------- Interactive content
<audio>
<canvas>
<embed>
<iframe>
<img>
<math>
<object>
<picture>
<svg>
<video>

*/

/*NOTE: ---------- Block level content
<address>
<article>
<aside>
<blockquote>
<details>
<dialog>
<div>
<dl>
<fieldset>
<figure>
<footer>
<form>
<header>
<h1>
<h2>
<h3>
<h4>
<h5>
<h6>
<hr>
<li>
<main>
<nav>
<ol>
<p>
<pre>
<section>
<table>
<ul>
*/

/*NOTE: ---------- Inline level content
<a>
<abbr>
<area> (if it is a descendant of a <map> element)
<audio>
<b>
<bdi>
<bdo>
<br>
<button>
<canvas>
<cite>
<code>
<data>
<datalist>
<dfn>
<em>
<embed>
<i>
<iframe>
<img>
<input>
<kbd>
<label>
<link> (if it is a descendant of a <noscript> element or it has an itemprop attribute)
<map> (if it is a descendant of a <noscript> element)
<mark>
<math>
<meta> (if it is a descendant of a <noscript> element)
<meter>
<noscript> (if it is a descendant of a <noscript> element)
<object>
<output>
<picture>
<progress>
<q>
<ruby>
<s>
<samp>
<script>
<select>
<small>
<span>
<strong>
<sub>
<sup>
<svg>
<template>
<textarea>
<time>
<u>
<var>
<video>
<wbr>

*/
/*NOTE: ---------- Inline level content Scripting
<canvas>
<noframes>
<noscript>
<script>
*/

/*NOTE: ---------- Metadata content
<base>
<link>
<meta>
<noscript>
<script>
<style>
<title>
*/

/*NOTE: ---------- 
Parent Element Content Model:

<p> (Paragraph) Element: Can contain phrasing content. It cannot contain block-level elements like headings, sections, etc.
<h1> to <h6> (Heading) Elements: Can only contain phrasing content.
<div> Element: Can contain flow content, which includes both block-level and inline-level elements.
<span> Element: Can only contain phrasing content.
*/
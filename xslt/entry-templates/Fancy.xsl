<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:tc="http://periapsis.org/tellico/"
                exclude-result-prefixes="tc"
                version="1.0">

<!--
   ===================================================================
   Tellico XSLT file - fancy template for viewing entry data

   $Id: Fancy.xsl 977 2004-11-26 07:33:15Z robby $

   Copyright (C) 2003, 2004 Robby Stephenson - robby@periapsis.org

   The drop-shadow effect is based on the "A List Apart" method
   at http://www.alistapart.com/articles/cssdropshadows/

   This XSLT stylesheet is designed to be used with the 'Tellico'
   application, which can be found at http://www.periapsis.org/tellico/
   ===================================================================
-->

<!-- import common templates -->
<!-- location depends on being installed correctly -->
<xsl:import href="../tellico-common.xsl"/>

<xsl:output method="html" version="xhtml"/>

<xsl:param name="datadir"/> <!-- dir where Tellico data are located -->
<xsl:param name="imgdir"/> <!-- dir where field images are located -->
<xsl:param name="font"/> <!-- default KDE font family -->
<xsl:param name="fgcolor"/> <!-- default KDE foreground color -->
<xsl:param name="bgcolor"/> <!-- default KDE background color -->
<xsl:param name="color1"/> <!-- default KDE highlighted text color -->
<xsl:param name="color2"/> <!-- default KDE highlighted background color -->

<xsl:param name="collection-file"/> <!-- might have a link to parent collection -->

<xsl:key name="fieldsByName" match="tc:field" use="@name"/>
<xsl:key name="fieldsByCat" match="tc:field" use="@category"/>
<xsl:key name="imagesById" match="tc:image" use="@id"/>

<xsl:variable name="image-width" select="'150'"/>
<xsl:variable name="image-height" select="'200'"/>
<xsl:variable name="endl">
<xsl:text>
</xsl:text>
</xsl:variable>

<!-- all the categories -->
<xsl:variable name="categories" select="/tc:tellico/tc:collection/tc:fields/tc:field[generate-id(.)=generate-id(key('fieldsByCat',@category)[1])]/@category"/>
<!-- layout changes depending on whether there are images or not -->
<xsl:variable name="num-images" select="count(tc:tellico/tc:collection/tc:entry[1]/*[key('fieldsByName',local-name(.))/@type = 10])"/>

<xsl:template match="/">
 <xsl:apply-templates select="tc:tellico"/>
</xsl:template>

<!-- The default layout is pretty boring, but catches every field value in
     the entry. The title is in the top H1 element. -->
<xsl:template match="tc:tellico">
 <!-- This stylesheet is designed for Tellico document syntax version 7 -->
 <xsl:call-template name="syntax-version">
  <xsl:with-param name="this-version" select="'7'"/>
  <xsl:with-param name="data-version" select="@syntaxVersion"/>
 </xsl:call-template>

 <html>
  <head>
  <style type="text/css">
  body {
    margin: 0px;
    padding: 0px;
    font-family: "<xsl:value-of select="$font"/>";
    color: <xsl:value-of select="$fgcolor"/>;
    background-color: <xsl:value-of select="$bgcolor"/>;
  }
  h1 {
    margin: 0px;
    padding: 0px;
    font-size: 1.8em;
    color: <xsl:value-of select="$color1"/>;
    background-color: <xsl:value-of select="$color2"/>;
/*    border: 1px solid;
    border-color: <xsl:value-of select="$fgcolor"/>;*/
    border-bottom: 1px outset black;
    text-align: center;
  }
  <xsl:if test="$num-images &gt; 0">
  #content {
   padding-right: <xsl:value-of select="$image-width + 10"/>px;
  }
  #images {
    margin: 10px 5px 0px 5px;
    float: right;
    min-width: <xsl:value-of select="$image-width"/>px; /* min-width instead of width, stylesheet actually scales image */
  }
  </xsl:if>
  div.img-shadow {
    float: left;
    background: url(<xsl:value-of select="concat($datadir,'shadowAlpha.png')"/>) no-repeat bottom right;
    margin: 6px 0 10px 10px;
    clear: left;
  }
  div.img-shadow img {
    display: block;
    position: relative;
    border: 1px solid #a9a9a9;
    margin: -6px 6px 6px -6px;
  }
  h3 {
    text-align: center;
    font-size: 1.1em;
  }
  div.category {
    padding: 0px 0px 0px 4px;
    margin: 10px;
    border: 1px inset <xsl:value-of select="$fgcolor"/>;
    text-align: center;
    background-color: #cccccc;
    min-height: 1em;
    overflow: hidden;
  }
  h2 {
    color: <xsl:value-of select="$color1"/>;
    background-color: <xsl:value-of select="$color2"/>;
    border-bottom: 1px outset;
    border-color: <xsl:value-of select="$fgcolor"/>;
    padding: 0px 5px 0px 0px;
    margin: 0px 0px 2px -5px;
    font-size: 1.0em;
    top: -1px;
    font-style: normal;
    text-align: right;
  }
  table {
    border-collapse: collapse;
    border-spacing: 0px;
    max-width: 100%;
  }
  th.fieldName {
    font-weight: bolder;
    text-align: left;
    padding: 0px 4px 0px 2px;
    white-space: nowrap;
  }
  td.fieldValue {
    text-align: left;
    padding: 0px 10px 0px 2px;
    width: 90%; /* nowrap is set on the fieldName column, so just want enough width to take the rest */
  }
  td.column1 {
    font-weight: bold;
    text-align: left;
    padding: 0px 2px 0px 2px;
    white-space: nowrap;
  }
  td.column2 {
    font-style: italic;
    text-align: left;
    padding: 0px 10px 0px 10px;
    width: 90%; /* nowrap is set on the fieldName column, so just want enough width to take the rest */
  }
  p {
    margin: 2px 0px 2px 0;
    padding: 0px;
    text-align: left;
  }
  ul {
    text-align: left;
    margin: 0px 0px 0px 0px;
    padding-left: 20px;
  }
  </style>
  <title>
   <xsl:value-of select="tc:collection/tc:entry[1]//tc:title[1]"/>
   <xsl:text> - </xsl:text>
   <xsl:value-of select="tc:collection/@title"/>
  </title>
  </head>
  <body>
   <xsl:apply-templates select="tc:collection"/>
  </body>
 </html>
</xsl:template>

<xsl:template match="tc:collection">
 <xsl:apply-templates select="tc:entry[1]"/>
 <xsl:if test="$collection-file">
  <hr/>
  <h4 style="text-align:center"><a href="{$collection-file}">&lt;&lt; <xsl:value-of select="@title"/></a></h4>
 </xsl:if>
</xsl:template>

<xsl:template match="tc:entry">
 <xsl:variable name="entry" select="."/>

 <!-- first, show the title -->
 <xsl:if test=".//tc:title">
  <h1>
   <xsl:value-of select=".//tc:title[1]"/>
  </h1>
 </xsl:if>

 <!-- all the images are in a div, aligned to the right side and floated -->
 <!-- only want this div if there are any images in the entry -->
 <xsl:if test="$num-images &gt; 0">
  <div id="images">
   <!-- images are field type 10 -->
   <xsl:for-each select="../tc:fields/tc:field[@type=10]">
    
    <!-- find the value of the image field in the entry -->
    <xsl:variable name="image" select="$entry/*[local-name(.) = current()/@name]"/>
    <!-- check if the value is not empty -->
    <xsl:if test="$image">
     <div class="img-shadow">
      <a>
       <xsl:attribute name="href">
        <xsl:choose>
         <!-- Amazon license requires the image to be linked to the amazon website -->
         <xsl:when test="$entry/tc:amazon">
          <xsl:value-of select="$entry/tc:amazon"/>
         </xsl:when>
         <xsl:otherwise>
          <xsl:value-of select="concat($imgdir, $image)"/>
         </xsl:otherwise>
        </xsl:choose>
       </xsl:attribute>
       <img>
        <xsl:attribute name="src">
         <xsl:value-of select="concat($imgdir, $image)"/>
        </xsl:attribute>
        <!-- limit to maximum width of 150 and height of 200 -->
        <xsl:call-template name="image-size">
         <xsl:with-param name="limit-width" select="$image-width"/>
         <xsl:with-param name="limit-height" select="$image-height"/>
         <xsl:with-param name="image" select="key('imagesById', $image)"/>
        </xsl:call-template>
       </img>
      </a>
     </div>
     <br/> <!-- needed since the img-shadow block floats -->
    </xsl:if>
   </xsl:for-each>
  </div>
 </xsl:if>
 
 <!-- all the data is in the content block -->
 <div id="content">
  <!-- now for all the rest of the data -->
  <!-- iterate over the categories, but skip paragraphs and images -->
  <xsl:for-each select="$categories[key('fieldsByCat',.)[1]/@type != 2 and key('fieldsByCat',.)[1]/@type != 10]">
   <xsl:call-template name="output-category">
    <xsl:with-param name="entry" select="$entry"/>
    <xsl:with-param name="category" select="."/>
   </xsl:call-template>
  </xsl:for-each>

  <!-- now do paragraphs -->
  <xsl:for-each select="$categories[key('fieldsByCat',.)[1]/@type = 2]">
   <xsl:call-template name="output-category">
    <xsl:with-param name="entry" select="$entry"/>
    <xsl:with-param name="category" select="."/>
   </xsl:call-template>
  </xsl:for-each>

 </div>

</xsl:template>

<xsl:template name="output-category">
 <xsl:param name="entry"/>
 <xsl:param name="category"/>

 <xsl:variable name="fields" select="key('fieldsByCat', $category)"/>
 <xsl:variable name="first-type" select="$fields[1]/@type"/>

 <xsl:variable name="n" select="count($entry//*[key('fieldsByName',local-name(.))/@category=$category])"/>
 <xsl:if test="$n &gt; 0 or $num-images = 0">
  <div class="category">
   
   <xsl:if test="$num-images = 0 and $first-type != 2">
    <xsl:attribute name="style">
     <xsl:text>float: left; margin-left: 1%; margin-right: 1%; width: 47%;</xsl:text>
     <!-- two columns of divs -->
     <xsl:if test="$categories[. = $category and position() mod 2 = 1]">
      <xsl:text>clear: left;</xsl:text>
     </xsl:if>
    </xsl:attribute>
   </xsl:if>
   <xsl:if test="$num-images = 0 and $first-type = 2">
    <xsl:attribute name="style">
     <xsl:text>clear: left;</xsl:text>
    </xsl:attribute>
   </xsl:if>
   
   <h2>
    <xsl:value-of select="$category"/>
   </h2>
   <!-- ok, big xsl:choose loop for field type -->
   <xsl:choose>
    
    <!-- paragraphs are field type 2 -->
    <xsl:when test="$first-type = 2">
     <p>
      <!-- disabling the output escaping allows html -->
      <xsl:value-of select="$entry/*[local-name(.) = $fields[1]/@name]" disable-output-escaping="yes"/>
     </p>
    </xsl:when>
    
    <!-- single-column tables are field type 8 -->
    <!-- ok to put category name inside div instead of table here -->
    <xsl:when test="$first-type = 8">
     <ul>
      <xsl:for-each select="$entry//*[local-name(.) = $fields[1]/@name]">
       <li>
        <xsl:value-of select="."/>
       </li>
      </xsl:for-each>
     </ul>
    </xsl:when>
    
    <!-- double-column tables are field type 9 -->
    <xsl:when test="$first-type = 9">
     <table>
      <tbody>
       <xsl:for-each select="$entry//*[local-name(.) = $fields[1]/@name]">
        <tr>
         <td class="column1">
          <xsl:value-of select="tc:column[1]"/>
         </td>
         <td class="column2">
          <xsl:value-of select="tc:column[2]"/>
         </td>
        </tr>
       </xsl:for-each>
      </tbody>
     </table>
    </xsl:when>

    <xsl:otherwise>
     <table>
      <tbody>
       <!-- already used title, so skip it -->
       <xsl:for-each select="$fields[@name != 'title']">
        <tr>
         <th class="fieldName" valign="top">
          <xsl:value-of select="@title"/>
          <xsl:text>:</xsl:text>
         </th>
         <td class="fieldValue">
          <xsl:call-template name="simple-field-value">
           <xsl:with-param name="entry" select="$entry"/>
           <xsl:with-param name="field" select="@name"/>
          </xsl:call-template>
         </td>
        </tr>
       </xsl:for-each>
      </tbody>
     </table>
     
    </xsl:otherwise>
   </xsl:choose>
   
  </div>
 </xsl:if> 
</xsl:template>
 
</xsl:stylesheet>
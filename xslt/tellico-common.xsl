<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:tc="http://periapsis.org/tellico/"
                exclude-result-prefixes="tc"
                version="1.0">

<!--
   ===================================================================
   Tellico XSLT file - some common templates.

   $Id: tellico-common.xsl 977 2004-11-26 07:33:15Z robby $

   Copyright (C) 2004 Robby Stephenson - robby@periapsis.org

   This XSLT stylesheet is designed to be used with the 'Tellico'
   application, which can be found at http://www.periapsis.org/tellico/
   ===================================================================
-->

<xsl:output method="html"/>

<!-- Template for checking syntax version -->
<xsl:template name="syntax-version">
 <xsl:param name="this-version"/>
 <xsl:param name="data-version"/>
 <xsl:if test="$data-version &gt; $this-version">
  <xsl:message>
   <xsl:text>This stylesheet was designed for Tellico DTD version </xsl:text>
   <xsl:value-of select="$this-version"/>
   <xsl:text> or earlier, &#xa;but the input data file is version </xsl:text>
   <xsl:value-of select="$data-version"/>
   <xsl:text>. There might be some &#xa;problems with the output.</xsl:text>
  </xsl:message>
 </xsl:if>
</xsl:template>

<!-- template for creating attributes to scale an image to a max boundary size -->
<xsl:template name="image-size">
 <xsl:param name="limit-height"/>
 <xsl:param name="limit-width"/>
 <xsl:param name="image"/>

 <xsl:variable name="actual-width" select="$image/@width"/>
 <xsl:variable name="actual-height" select="$image/@height"/>

 <xsl:choose>
  <xsl:when test="$actual-width &gt; $limit-width or $actual-height &gt; $limit-height">

   <!-- -->
   <xsl:choose>

    <xsl:when test="$actual-width * $limit-height &lt; $actual-height * $limit-width">
     <xsl:attribute name="height">
      <xsl:value-of select="$limit-height"/>
     </xsl:attribute>
     <xsl:attribute name="width">
      <xsl:value-of select="round($actual-width * $limit-height div $actual-height)"/>
     </xsl:attribute>
    </xsl:when>

    <xsl:otherwise>
     <xsl:attribute name="width">
      <xsl:value-of select="$limit-width"/>
     </xsl:attribute>
     <xsl:attribute name="height">
      <xsl:value-of select="round($actual-height * $limit-width div $actual-width)"/>
     </xsl:attribute>
    </xsl:otherwise>

   </xsl:choose>

  </xsl:when>

  <!-- if both are smaller, no change -->
  <xsl:otherwise>
   <xsl:attribute name="height">
    <xsl:value-of select="$actual-height"/>
   </xsl:attribute>
   <xsl:attribute name="width">
    <xsl:value-of select="$actual-width"/>
   </xsl:attribute>
  </xsl:otherwise>

 </xsl:choose>
</xsl:template>

<!-- template for outputing most value types -->
<xsl:template name="simple-field-value">
 <xsl:param name="entry"/>
 <xsl:param name="field"/>

 <!-- if the field has multiple values, then there is
      no child of the entry with the field name -->
 <xsl:variable name="child" select="$entry/*[local-name(.)=$field]"/>
 <xsl:choose>
  <xsl:when test="$child">
   <xsl:variable name="f" select="key('fieldsByName',$field)"/>

   <!-- if the field is a bool type, just ouput an X -->
   <xsl:choose>
    <xsl:when test="$f/@type=4">
     <img height="14">
      <xsl:attribute name="src">
       <xsl:value-of select="concat($datadir,'pics/checkmark.png')"/>
      </xsl:attribute>
     </img>
    </xsl:when>

    <!-- if it's a url, then add a hyperlink -->
    <xsl:when test="$f/@type=7">
     <a href="{$child}">
      <!-- The Amazon Web Services license requires the link -->
      <xsl:choose>
       <xsl:when test="$field = 'amazon'">
        <xsl:text>Buy from Amazon.com</xsl:text>
       </xsl:when>
       <xsl:otherwise>
        <xsl:value-of select="$child"/>
       </xsl:otherwise>
      </xsl:choose>
     </a>
    </xsl:when>

    <!-- if it's a date, format with hyphens -->
    <xsl:when test="$f/@type=12">
     <xsl:value-of select="$child/tc:year"/>
     <xsl:text>-</xsl:text>
     <xsl:value-of select="$child/tc:month"/>
     <xsl:text>-</xsl:text>
     <xsl:value-of select="$child/tc:day"/>
    </xsl:when>

    <!-- special case for rating -->
    <xsl:when test="$f/@type=3 and ($f/@name='rating' or $f/tc:prop[@name='rating']='true')">
     <!-- get the number, could be 10, so can't just grab first digit -->
     <xsl:variable name="n">
      <xsl:choose>
       <xsl:when test="number(substring($child,1,1))">
        <xsl:choose>
         <xsl:when test="number(substring($child,1,2)) &lt; 11">
          <xsl:value-of select="number(substring($child,1,2))"/>
         </xsl:when>
         <xsl:otherwise>
          <xsl:value-of select="number(substring($child,1,1))"/>
         </xsl:otherwise>
        </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
        <xsl:value-of select="false()"/>
       </xsl:otherwise>
      </xsl:choose>
     </xsl:variable>
     <xsl:if test="$n">
      <!-- the image is really 18 pixels high, but make it smaller to match default font -->
      <img height="14">
       <xsl:attribute name="src">
        <xsl:value-of select="concat($datadir,'pics/stars',$n,'.png')"/>
       </xsl:attribute>
      </img>
     </xsl:if>
     <xsl:if test="not($n)">
      <xsl:value-of select="$child"/>     
     </xsl:if>
    </xsl:when>
    
    <xsl:otherwise>
     <xsl:value-of select="$child"/>
     <!-- hack for running-time in videos -->
     <!--
     <xsl:if test="$field='running-time' and key('fieldsByName',$field)/@type=6">
      <xsl:text> minutes</xsl:text>
     </xsl:if>
     -->
    </xsl:otherwise>
   </xsl:choose>
  </xsl:when>

  <!-- now handle fields with multiple values -->
  <xsl:otherwise>
   <xsl:for-each select="$entry/*[local-name(.)=concat($field,'s')]/*">
    <xsl:value-of select="."/>
    <xsl:if test="position() != last()">
     <xsl:text>; </xsl:text>
    </xsl:if>
   </xsl:for-each>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template name="msqueeze">
 <xsl:param name="str"/>
 <xsl:param name="len"/>

 <xsl:variable name="slen" select="string-length($str)"/>
 <xsl:variable name="mid" select="floor($len div 2)"/>

 <xsl:choose>
  <xsl:when test="$slen &gt; $len">
   <xsl:value-of select="substring($str, 0, $mid - 2)"/>
   <xsl:text>[...]</xsl:text>
   <xsl:value-of select="substring($str, $slen - $mid + 3, $mid - 2)"/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$str"/>   
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<!-- sums all nodes, assuming they are in MM:SS format -->
<xsl:template name="sumTime">
 <xsl:param name="nodes" select="/.."/>
 <xsl:param name="totalMin" select="0"/>
 <xsl:param name="totalSec" select="0"/>

 <xsl:choose>

  <xsl:when test="not($nodes)">
   <xsl:value-of select="$totalMin + round($totalSec div 60)"/>
   <xsl:text>:</xsl:text>
   <xsl:value-of select="format-number($totalSec mod 60, '00')"/>
  </xsl:when>

  <xsl:when test="string-length($nodes[1]) &gt; 0">
   <xsl:variable name="min">
    <xsl:value-of select="substring-before($nodes[1], ':')"/>
   </xsl:variable>
   <xsl:variable name="sec">
    <xsl:value-of select="substring-after($nodes[1], ':')"/>
   </xsl:variable>
   <xsl:call-template name="sumTime">
    <xsl:with-param name="nodes" select="$nodes[position() != 1]"/>
    <xsl:with-param name="totalMin" select="$totalMin + $min"/>
    <xsl:with-param name="totalSec" select="$totalSec + $sec"/>
   </xsl:call-template>
  </xsl:when>

  <xsl:otherwise>
   <xsl:call-template name="sumTime">
    <xsl:with-param name="nodes" select="$nodes[position() != 1]"/>
    <xsl:with-param name="totalMin" select="$totalMin"/>
    <xsl:with-param name="totalSec" select="$totalSec"/>
   </xsl:call-template>   
  </xsl:otherwise>

 </xsl:choose>
</xsl:template>

</xsl:stylesheet>
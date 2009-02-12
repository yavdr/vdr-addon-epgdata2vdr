<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/TR/xhtml1/strict"
                xmlns:exsl="http://exslt.org/common"
                xmlns:date="http://exslt.org/dates-and-times"
                xmlns:str="http://exslt.org/strings">

<xsl:output method="text" omit-xml-declaration="yes" encoding='iso-8859-1'/>
<!-- 
<xsl:output method="text" omit-xml-declaration="yes" encoding='utf-8'/>
-->


<xsl:template match="/"> 
  <xsl:for-each select="stations/channel">
  <!-- C section -->
  <xsl:text>C </xsl:text> 
  <xsl:value-of select="vdrchannelid"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="name"/>
  <xsl:text>&#10;</xsl:text> 
  <!-- events -->
     <xsl:call-template name="epgdata">
      <xsl:with-param name="file" select="$filename"/>
      <xsl:with-param name="chan" select="epgdatacomid"/>
     </xsl:call-template>
  <xsl:text>c </xsl:text>
 </xsl:for-each> 
</xsl:template>

<xsl:template name="epgdata">
  <xsl:param name="file"/>
  <xsl:param name="chan"/>
   <xsl:for-each select="document($file)/pack/data">
    <xsl:choose>
      <xsl:when test="d2 = $chan">
      <!-- START parse content into variables -->
      <xsl:variable name="broadcast_id">
            <xsl:value-of select="d0"/>
      </xsl:variable>
      <xsl:variable name="tvshow_id">
            <xsl:value-of select="d1"/>
      </xsl:variable>
      <xsl:variable name="tvchannel_id">
            <xsl:value-of select="d2"/>
      </xsl:variable>
      <xsl:variable name="tvregionid">
            <xsl:value-of select="d3"/>
      </xsl:variable>
      <xsl:variable name="starttime">
            <xsl:value-of select="d4"/>
      </xsl:variable>
      <xsl:variable name="endtime">
            <xsl:value-of select="d5"/>
      </xsl:variable>
      <xsl:variable name="broadcast_day">
            <xsl:value-of select="d6"/>
      </xsl:variable>
      <xsl:variable name="tvshow_length">
            <xsl:value-of select="d7"/>
      </xsl:variable>
      <xsl:variable name="vps">
            <xsl:value-of select="d8"/>
      </xsl:variable>
      <xsl:variable name="primetime">
            <xsl:value-of select="d9"/>
      </xsl:variable>
      <xsl:variable name="category_id">
            <xsl:value-of select="d10"/>
      </xsl:variable>
      <xsl:variable name="technics_bw">
            <xsl:value-of select="d11"/>
      </xsl:variable>
      <xsl:variable name="technics_co_channel">
            <xsl:value-of select="d12"/>
      </xsl:variable>
      <xsl:variable name="technics_vt150">
            <xsl:value-of select="d13"/>
      </xsl:variable>
      <xsl:variable name="technics_coded">
            <xsl:value-of select="d14"/>
      </xsl:variable>
      <xsl:variable name="technics_blind">
            <xsl:value-of select="d15"/>
      </xsl:variable>
      <xsl:variable name="age_marker">
            <xsl:value-of select="d16"/>
      </xsl:variable>
      <xsl:variable name="live_id">
            <xsl:value-of select="d17"/>
      </xsl:variable>
      <xsl:variable name="tipflag">
            <xsl:value-of select="d18"/>
      </xsl:variable>
      <xsl:variable name="title">
            <xsl:value-of select="d19"/>
      </xsl:variable>
      <xsl:variable name="subtitle">
            <xsl:value-of select="d20"/>
      </xsl:variable>
      <xsl:variable name="comment_long">
            <xsl:value-of select="d21"/>
      </xsl:variable>
      <xsl:variable name="comment_middle">
            <xsl:value-of select="d22"/>
      </xsl:variable>
      <xsl:variable name="comment_short">
            <xsl:value-of select="d23"/>
      </xsl:variable>
      <xsl:variable name="themes">
            <xsl:value-of select="d24"/>
      </xsl:variable>
      <xsl:variable name="genreid">
            <xsl:value-of select="d25"/>
      </xsl:variable>
      <xsl:variable name="sequence">
            <xsl:value-of select="d26"/>
      </xsl:variable>
      <xsl:variable name="technics_stereo">
            <xsl:value-of select="d27"/>
      </xsl:variable>
      <xsl:variable name="technics_dolby">
            <xsl:value-of select="d28"/>
      </xsl:variable>
      <xsl:variable name="technics_wide">
            <xsl:value-of select="d29"/>
      </xsl:variable>
      <xsl:variable name="tvd_total_value">
            <xsl:value-of select="d30"/>
      </xsl:variable>
      <xsl:variable name="attribute">
            <xsl:value-of select="d31"/>
      </xsl:variable>
      <xsl:variable name="country">
            <xsl:value-of select="d32"/>
      </xsl:variable>
      <xsl:variable name="year">
            <xsl:value-of select="d33"/>
      </xsl:variable>
      <xsl:variable name="moderator">
            <xsl:value-of select="d34"/>
      </xsl:variable>
      <xsl:variable name="studio_guest">
            <xsl:value-of select="d35"/>
      </xsl:variable>
      <xsl:variable name="regisseur">
            <xsl:value-of select="d36"/>
      </xsl:variable>
      <xsl:variable name="actor">
            <xsl:value-of select="d37"/>
      </xsl:variable>
      <xsl:variable name="image_small">
            <xsl:value-of select="d38"/>
      </xsl:variable>
      <xsl:variable name="image_middle">
            <xsl:value-of select="d39"/>
      </xsl:variable>
      <xsl:variable name="image_big">
            <xsl:value-of select="d40"/>
      </xsl:variable>
      <!-- END parse content in variables -->
	<!-- output event block -->
         <xsl:text>E </xsl:text>
	 <xsl:value-of select="$tvshow_id"/>
         <xsl:text> </xsl:text>
	 <xsl:value-of select="date:seconds(str:replace($starttime,' ','T'))"/>
         <xsl:text> </xsl:text>
         <xsl:value-of select="$tvshow_length*60"/>
         <xsl:text> 0 0</xsl:text>
	 <xsl:text>&#10;</xsl:text>
         
	<!-- T title -->
         <xsl:text>T </xsl:text>
         <xsl:value-of select="$title"/>
         <xsl:text>&#10;</xsl:text>

	<!-- S summary -->
         <xsl:text>S </xsl:text>
         <xsl:value-of select="$subtitle"/>
         <xsl:text>&#10;</xsl:text>

	<!-- D description -->
         <xsl:text>D </xsl:text>
         <xsl:value-of select="$comment_long"/>
         <xsl:text>&#10;</xsl:text>

	<!-- X stream description 
         <xsl:text>X </xsl:text>
         <xsl:text>&#10;</xsl:text> -->

	<!-- V VPS -->
         <xsl:if test="string-length($vps) &gt; 1">
         <xsl:text>V </xsl:text>
         <xsl:value-of select="date:seconds(concat(substring-before($starttime,' '),'T',$vps))"/>
         <xsl:text>&#10;</xsl:text>
         </xsl:if>

         <xsl:text>e</xsl:text>
         <xsl:text>&#10;</xsl:text>
   </xsl:when>
   <xsl:otherwise/>
  </xsl:choose>
  </xsl:for-each>
</xsl:template>
</xsl:stylesheet>


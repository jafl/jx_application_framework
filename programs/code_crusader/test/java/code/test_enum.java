package com.yahoo.satg.common;

import java.util.Locale;
import java.util.HashMap;

import com.yahoo.satg.common.hibernate.H8ShortEnumBase;

/*****************************************************************************
 * Define constants associated with languages
 * 
 * @author John Lindal
 */

public enum Language
{
	ENGLISH("en"),
	GERMAN("de"),
	FRENCH("fr"),
	JAPANESE("ja"),
	KOREAN("ko"),
	ITALIAN("it"),
	SPANISH("es"),
	DUTCH("nl"),
	NORWEGIAN("no"),
	DANISH("da"),
	SWEDISH("sv"),
	FINNISH("fi"),
	PORTUGESE("pt"),
	CHINESE("zh");

	private static HashMap<Short, Language> theValueMapping;
	private static HashMap<String, Language> theISOMapping;
	
	private final Short		itsValue;
	private final String	itsISO639String;

	static
	{
		H8ShortEnumBase.registerMap(getClassForRegister(), getValueMapping());
	}

	private Language
		(
		String languageString
		)
	{
		itsValue        = (short) ordinal();
		itsISO639String = languageString;

		getValueMapping().put(itsValue, this);
		getISOMapping().put(itsISO639String, this);
	}

	private static HashMap<Short, Language> getValueMapping()
	{
		if (theValueMapping == null)
		{
			theValueMapping = new HashMap<Short, Language>();
		}
		return theValueMapping;
	}

	private static HashMap<String, Language> getISOMapping()
	{
		if (theISOMapping == null)
		{
			theISOMapping = new HashMap<String, Language>();
		}
		return theISOMapping;
	}

	private static Class getClassForRegister()
	{
		return Language.class;
	}

	/***************************************************************************
	 * Returns the value.
	 */

	public Short getValue()
	{
		return itsValue;
	}

	/***************************************************************************
	 * Returns the value.
	 */

	public short shortValue()
	{
		return itsValue.shortValue();
	}

	/*****************************************************************************
	 * Returns the ISO639 language string
	 *
	 * @return	String representing the language
	 */
	public String getISO639String()
	{
		return itsISO639String;
	}

	/*****************************************************************************
	 * Returns the Language for the given ISO639 language string
	 *
	 * @return	String representing the language
	 */
	public static Language get(
		String iso639String)
	{
		return theISOMapping.get(iso639String);
	}

	/*****************************************************************************
	 * Returns the Language for the given Locale
	 *
	 * @return	String representing the language
	 */
	public static Language get(
		Locale locale)
	{
		String lang = locale.getLanguage();
		if (lang.length() > 0)
		{
			return get(lang);
		}
		else
		{
			return ENGLISH;
		}
	}

	/*****************************************************************************
	 * Returns the matching <code>Language</code> object
	 *
	 * @param value	The value
	 * @return		The matching <code>Language</code> object
	 */
	public static Language get(
		short value)
	{
		return get(new Short(value));
	}

	/*****************************************************************************
	 * Returns the matching <code>Language</code> object
	 *
	 * @param value	The value
	 * @return		The matching <code>Language</code> object
	 */
	public static Language get(
		Short value)
	{
		return theValueMapping.get(value);
	}

}

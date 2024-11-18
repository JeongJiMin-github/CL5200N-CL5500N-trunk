 /* 
 * File:   UN_function.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���� ���� ��� ���� ���� (���� ���)
 * Created 2024/06/05
 */

#ifndef UN_function_H
#define	UN_function_H

//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						          // Use European Traceability
 #define USE_NUTRITION_FACT						        // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				    // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			  // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		  // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						        // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //	#define USE_REMOTE_KEY
 //	#define USE_REMOTE_DISP
 //	#define USE_ADMIN_EMAIL

//----------   ���� ���� ���(���� ���� X)  ----------//
  #ifdef USE_EUROPE_WITHOUT_TICKET
    #define DEALER_VER_STRING	"(u)"
		#define USE_ENG_LABELFORMAT					      // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
		#define USE_KOR_LABELFORMAT					      // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
		#define USE_ONLY_LABEL
  #elif defined(USE_STANDARD_DIRECT_INGREDIENT)
    #define DEALER_VER_STRING	"(D)"
    #define USE_TAX_RATE						          // Use Tax Rate
    #define USE_REPORT_TAX						        // TAX Report ���
    #define USE_REPORT_PAY						        // PAY/CHANGE Report ���
    #define USE_DIRECT_INGREDIENT				      // Use direct ingredient
    #define USE_INGREDIENT_98_PROTOCOL			  // PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
    //	#define USE_INGREDIENT_99_PROTOCOL		// PLU ��� �� ��� Direct ingredient�� 99�� ���(protocol �Ŵ��� ����)
    #define USE_ENG_LABELFORMAT					      // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
    #define USE_KOR_LABELFORMAT					      // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
    //----------   �̱�, ĳ����, ȣ���� ��� ���� �� ���� ���  ----------//
      #if defined  (COUNTRY_USA) || defined(COUNTRY_CANADA) || defined(COUNTRY_AUSTRALIA)
        #undef USE_ENG_LABELFORMAT
        #undef USE_KOR_LABELFORMAT
      #endif
  #else
    #define DEALER_VER_STRING	""
    #define USE_TAX_RATE						          // Use Tax Rate
    #define USE_REPORT_TAX						        // TAX Report ���
    #define USE_REPORT_PAY						        // PAY/CHANGE Report ���
    #ifdef USE_EXTEND_8M_DFLASH
      #define USE_LABEL_FORMAT_EXT 			      // �� ���� Ȯ��(4~8MB ����, 20�� �߰� Ȯ��)
    #endif
    #define USE_ENG_LABELFORMAT				        // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
    #define USE_KOR_LABELFORMAT				        // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
  #endif /* USE_EUROPE_WITHOUT_TICKET */
  #ifndef  COUNTRY_STR
    #define COUNTRY_STR "  "
  #endif
#endif  /* UN_function_H */
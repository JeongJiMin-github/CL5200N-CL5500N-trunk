 /* 
 * File:   IRN_p.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Persian(�̶�) ���� ����
 * Create 2024/06/04
 */

#ifndef IRN_p_H
#define	IRN_p_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  "(p)"
 #define COUNTRY_STR "  "
 #define USE_PERSIA_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_PERSIA_BRACKET                 // �̶��� ��ȣ �Է±� ó�� �߰�
 #define USE_TRACEABILITY		                // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_TAX_RATE			                  // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_REPORT_TAX			                // TAX Report ���
 #define USE_REPORT_PAY			                // PAY/CHANGE Report ���
 #define USE_PERSIAN_CALENDAR	              //�̶� �޷� ���
 #define USE_ARAB_CONVERT		                //�ƶ��� ��ȯ ���
 //#define USE_EXTEND_PRICE_PRIMARYSIGN	    // Use 5character price sign (Iran sign: Toman)
 #define USE_SEND_TICKET_DATA
 #define USE_ZERO_RANGE_MSG		              // ���� ���� �ʰ��� ���� ������ ��쿡 ��� �޽��� ���
 #define USE_ARAB_TICKET_PRT	              //���� Ƽ���� �Ϻθ� �ش� ���������� �и� �Ǿ��ְ�, �������� USE_ARAB_CONVERT �ȿ� ����Ǿ�����. ���� USE_ARAB_CONVERT �� ����� Ƽ�� ������ �и� �ʿ� 23.07.06
 #define USE_PERSIA_CAPTION
 #define USE_PERSIAN_SALE_AMOUNT_LIMIT		  // �Ѿ� ���� 
 #define USE_SUBTOTAL_PRICE_9DIGIT_LIMIT    // �ջ� �ݾ� 9�ڸ� ���ϸ� ��� 
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_EFT_POS_PROTOCOL			          // EFT-POS PROTOCOL ���(Remote IP2�� �������� ����)
  #ifdef USE_EFT_POS_PROTOCOL
    #undef USE_TRACE_STANDALONE			        // ����̷±��(����)������ Remote IP2�� ����ϹǷ� undef ó��
    #define USE_MAX_SCALE_NO_255			      // EFT-POS PROTOCOL���� SCALE ID 0~255���� ���
  #endif//USE_EFT_POS_PROTOCOL
 //#define USE_8_16_FONT_MENU_DISPLAY	      //���α׷� �޴� 8*16 Font Display ����
//----------   ���� ���� �� ����  ----------//
 #define USE_IRN_LABELFORMAT	              // �̶� ���� ������ ���(�⺻ �μ� ��Ʈ 6��Ÿ������ ���)
 #define USE_PERSIA_TICKET_PRT_FORMAT       //�̶� Ƽ�� ���
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
 #define USE_PRICE_UNIT_7DIGIT	            // Unit Price 7�ڸ� ���
 #define USE_PRICE_TOTAL_8DIGIT		          // Total 8�ڸ� ���
 #define USE_ARAB_FONT			                // �ƶ���Ʈ ���
 #define FONT_CODEPAGE CODEPAGE_1256_IRN
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// �ƶ��� ���� ������ LTR �������� �μ�
  #endif
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* IRN_p_H */
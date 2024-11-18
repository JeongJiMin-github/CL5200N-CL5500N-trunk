#include "globals.h"


#if !(ROM_COUNTRY == ROM_RUS)
////////////////////////////////////
// STANDARD LABEL FORMAT
////////////////////////////////////
#ifdef USE_ENG_LABELFORMAT

ROMDATA INT8U Label_Form01[]={
#include "Label_Form01.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_Form02.lfr"
};

ROMDATA INT8U Label_Form03[]={
#include "Label_Form03.lfr"
};

ROMDATA INT8U Label_Form04[]={
#include "Label_Form04.lfr"
};

ROMDATA INT8U Label_Form05[]={
#include "Label_Form05.lfr"
};

ROMDATA INT8U Label_Form06[]={
#include "Label_Form06.lfr"
};

ROMDATA INT8U Label_Form07[]={
#include "Label_Form07.lfr"
};

ROMDATA INT8U Label_Form08[]={
#include "Label_Form08.lfr"
};

ROMDATA INT8U Label_Form09[]={
#include "Label_Form09.lfr"
};

ROMDATA INT8U Label_Form10[]={
#include "Label_Form10.lfr"
};

ROMDATA INT8U Label_Form11[]={
#include "Label_Form11.lfr"
};

ROMDATA INT8U Label_Form12[]={
#include "Label_Form12.lfr"
};

ROMDATA INT8U Label_Form13[]={
#include "Label_Form13.lfr"
};

ROMDATA INT8U Label_Form14[]={
#include "Label_Form14.lfr"
};

ROMDATA INT8U Label_Form15[]={
#include "Label_Form15.lfr"
};

ROMDATA INT8U Label_Form16[]={
#include "Label_Form16.lfr"
};

ROMDATA INT8U Label_Form17[]={
#include "Label_Form17.lfr"
};

ROMDATA INT8U Label_Form18[]={
#include "Label_Form18.lfr"
};

ROMDATA INT8U Label_Form19[]={
#include "Label_Form19.lfr"
};

ROMDATA INT8U Label_Form20[]={
#include "Label_Form20.lfr"
};

ROMDATA INT8U Label_Form21[]={
#include "Label_Form21.lfr"
};

ROMDATA INT8U Label_Form22[]={
#include "Label_Form22.lfr"
};

ROMDATA INT8U Label_Form23[]={
#include "Label_Form23.lfr"
};

ROMDATA INT8U Label_Form24[]={
#include "Label_Form24.lfr"
};

ROMDATA INT8U Label_Form25[]={
#include "Label_Form25.lfr"
};

ROMDATA INT8U Label_Form26[]={
#include "Label_Form26.lfr"
};

ROMDATA INT8U Label_Form27[]={
#include "Label_Form27.lfr"
};

ROMDATA INT8U Label_Form28[]={
#include "Label_Form28.lfr"
};

#endif


// 국가에 상관 없는 공통 라벨 포멧(전용 라벨 폼 사용하는 이란, 호주, 미국/캐나다 제외
#if !defined(USE_AU_LABELFORMAT) && !defined(USE_IRN_LABELFORMAT) && !defined(USE_USA_LABELFORMAT)
ROMDATA INT8U Label_Form29[]={
#include "Label_Form29.lfr"
};

ROMDATA INT8U Label_Form30[]={
#include "Label_Form30.lfr"
};

ROMDATA INT8U Label_Form31[]={
#include "Label_Form31.lfr"
};
#endif

#ifdef USE_KOR_LABELFORMAT

ROMDATA INT8U Label_Form32[]={
#include "Label_Form32.lfr"
};

ROMDATA INT8U Label_Form33[]={
#include "Label_Form33.lfr"
};

ROMDATA INT8U Label_Form34[]={
#include "Label_Form34.lfr"
};

ROMDATA INT8U Label_Form35[]={
#include "Label_Form35.lfr"
};

ROMDATA INT8U Label_Form36[]={
#include "Label_Form36.lfr"
};

ROMDATA INT8U Label_Form37[]={
#include "Label_Form37.lfr"
};

ROMDATA INT8U Label_Form38[]={
#include "Label_Form38.lfr"
};

ROMDATA INT8U Label_Form39[]={
#include "Label_Form39.lfr"
};

ROMDATA INT8U Label_Form40[]={
#include "Label_Form40.lfr"
};

ROMDATA INT8U Label_Form41[]={
#include "Label_Form41.lfr"
};


ROMDATA INT8U Label_Form42[]={
#include "Label_Form42.lfr"
};


ROMDATA INT8U Label_Form43[]={
#include "Label_Form43.lfr"
};


ROMDATA INT8U Label_Form44[]={
#include "Label_Form44.lfr"
};

ROMDATA INT8U Label_Form45[]={
#include "Label_Form45.lfr"
};
#endif
////////////////////////////////////

#ifdef USE_CHN_LABELFORMAT
ROMDATA INT8U Label_Form01[]={
#include "Label_6040.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_4030.lfr"
};
#endif


#ifdef USE_AU_LABELFORMAT //호주 전용라벨
////////////////////////////////////
// AUSTRAILIA LABEL FORMAT
////////////////////////////////////
ROMDATA INT8U Label_Form01[]={
#include "Label_AU01.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_AU02.lfr"
};

ROMDATA INT8U Label_Form03[]={
#include "Label_AU03.lfr"
};

ROMDATA INT8U Label_Form04[]={
#include "Label_AU04.lfr"
};

ROMDATA INT8U Label_Form05[]={
#include "Label_AU05.lfr"
};

ROMDATA INT8U Label_Form06[]={
#include "Label_AU06.lfr"
};

ROMDATA INT8U Label_Form07[]={
#include "Label_AU07.lfr"
};

ROMDATA INT8U Label_Form08[]={
#include "Label_AU08.lfr"
};

ROMDATA INT8U Label_Form09[]={
#include "Label_AU09.lfr"
};

ROMDATA INT8U Label_Form10[]={
#include "Label_AU10.lfr"
};

ROMDATA INT8U Label_Form11[]={
#include "Label_AU11.lfr"
};

ROMDATA INT8U Label_Form12[]={
#include "Label_AU12.lfr"
};

ROMDATA INT8U Label_Form13[]={
#include "Label_AU13.lfr"
};

ROMDATA INT8U Label_Form14[]={
#include "Label_AU14.lfr"
};

ROMDATA INT8U Label_Form15[]={
#include "Label_AU15.lfr"
};

ROMDATA INT8U Label_Form16[]={
#include "Label_AU16.lfr"
};

ROMDATA INT8U Label_Form17[]={
#include "Label_AU17.lfr"
};

ROMDATA INT8U Label_Form18[]={
#include "Label_AU18.lfr"
};

ROMDATA INT8U Label_Form19[]={
#include "Label_AU19.lfr"
};

ROMDATA INT8U Label_Form20[]={
#include "Label_AU20.lfr"
};

ROMDATA INT8U Label_Form21[]={
#include "Label_AU21.lfr"
};

ROMDATA INT8U Label_Form22[]={
#include "Label_AU22.lfr"
};

ROMDATA INT8U Label_Form23[]={
#include "Label_AU23.lfr"
};

ROMDATA INT8U Label_Form24[]={
#include "Label_AU24.lfr"
};

ROMDATA INT8U Label_Form25[]={
#include "Label_AU25.lfr"
};

ROMDATA INT8U Label_Form26[]={
#include "Label_AU26.lfr"
};

ROMDATA INT8U Label_Form27[]={
#include "Label_AU27.lfr"
};

ROMDATA INT8U Label_Form28[]={
#include "Label_AU28.lfr"
};

ROMDATA INT8U Label_Form29[]={
#include "Label_AU29.lfr"
};

ROMDATA INT8U Label_Form30[]={
#include "Label_AU30.lfr"
};

ROMDATA INT8U Label_Form31[]={
#include "Label_AU31.lfr"
};

ROMDATA INT8U Label_Form32[]={
#include "Label_AU32.lfr"
};

ROMDATA INT8U Label_Form33[]={
#include "Label_AU33.lfr"
};

ROMDATA INT8U Label_Form34[]={
#include "Label_AU34.lfr"
};

ROMDATA INT8U Label_Form35[]={
#include "Label_AU35.lfr"
};

ROMDATA INT8U Label_Form36[]={
#include "Label_AU36.lfr"
};

ROMDATA INT8U Label_Form37[]={
#include "Label_AU37.lfr"
};

ROMDATA INT8U Label_Form38[]={
#include "Label_AU38.lfr"
};

ROMDATA INT8U Label_Form39[]={
#include "Label_AU39.lfr"
};

ROMDATA INT8U Label_Form40[]={
#include "Label_AU40.lfr"
};

ROMDATA INT8U Label_Form41[]={
#include "Label_AU41.lfr"
};

ROMDATA INT8U Label_Form42[]={
#include "Label_AU42.lfr"
};

ROMDATA INT8U Label_Form43[]={
#include "Label_AU43.lfr"
};

ROMDATA INT8U Label_Form44[]={
#include "Label_AU44.lfr"
};

ROMDATA INT8U Label_Form45[]={
#include "Label_AU45.lfr"
};

#ifdef USE_AUSTRALIA_COOL_FUNCTION
ROMDATA INT8U Image_Form_logo[]={
#include "Label_AU_Logo.lfr"
};

ROMDATA INT8U Image_Form_bar[]={
#include "Label_AU_Bar.lfr"
};
#endif  // USE_AUSTRALIA_COOL_FUNCTION
////////////////////////////////////
#endif // USE_AU_LABELFORMAT

#ifdef USE_IRN_LABELFORMAT
////////////////////////////////////
// IRAN LABEL FORMAT
////////////////////////////////////
ROMDATA INT8U Label_Form01[]={
#include "Label_IR01.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_IR02.lfr"
};

ROMDATA INT8U Label_Form03[]={
#include "Label_IR03.lfr"
};

ROMDATA INT8U Label_Form04[]={
#include "Label_IR04.lfr"
};

ROMDATA INT8U Label_Form05[]={
#include "Label_IR05.lfr"
};

ROMDATA INT8U Label_Form06[]={
#include "Label_IR06.lfr"
};

ROMDATA INT8U Label_Form07[]={
#include "Label_IR07.lfr"
};

ROMDATA INT8U Label_Form08[]={
#include "Label_IR08.lfr"
};

ROMDATA INT8U Label_Form09[]={
#include "Label_IR09.lfr"
};

ROMDATA INT8U Label_Form10[]={
#include "Label_IR10.lfr"
};

ROMDATA INT8U Label_Form11[]={
#include "Label_IR11.lfr"
};

ROMDATA INT8U Label_Form12[]={
#include "Label_IR12.lfr"
};

ROMDATA INT8U Label_Form13[]={
#include "Label_IR13.lfr"
};

ROMDATA INT8U Label_Form14[]={
#include "Label_IR14.lfr"
};

ROMDATA INT8U Label_Form15[]={
#include "Label_IR15.lfr"
};

ROMDATA INT8U Label_Form16[]={
#include "Label_IR16.lfr"
};

ROMDATA INT8U Label_Form17[]={
#include "Label_IR17.lfr"
};

ROMDATA INT8U Label_Form18[]={
#include "Label_IR18.lfr"
};

ROMDATA INT8U Label_Form19[]={
#include "Label_IR19.lfr"
};

ROMDATA INT8U Label_Form20[]={
#include "Label_IR20.lfr"
};

ROMDATA INT8U Label_Form21[]={
#include "Label_IR21.lfr"
};

ROMDATA INT8U Label_Form22[]={
#include "Label_IR22.lfr"
};

ROMDATA INT8U Label_Form23[]={
#include "Label_IR23.lfr"
};

ROMDATA INT8U Label_Form24[]={
#include "Label_IR24.lfr"
};

ROMDATA INT8U Label_Form25[]={
#include "Label_IR25.lfr"
};

ROMDATA INT8U Label_Form26[]={
#include "Label_IR26.lfr"
};

ROMDATA INT8U Label_Form27[]={
#include "Label_IR27.lfr"
};

ROMDATA INT8U Label_Form28[]={
#include "Label_IR28.lfr"
};

ROMDATA INT8U Label_Form29[]={
#include "Label_IR29.lfr"
};

ROMDATA INT8U Label_Form30[]={
#include "Label_IR30.lfr"
};

ROMDATA INT8U Label_Form31[]={
#include "Label_IR31.lfr"
};

ROMDATA INT8U Label_Form32[]={
#include "Label_IR32.lfr"
};

ROMDATA INT8U Label_Form33[]={
#include "Label_IR33.lfr"
};

ROMDATA INT8U Label_Form34[]={
#include "Label_IR34.lfr"
};

ROMDATA INT8U Label_Form35[]={
#include "Label_IR35.lfr"
};

ROMDATA INT8U Label_Form36[]={
#include "Label_IR36.lfr"
};

ROMDATA INT8U Label_Form37[]={
#include "Label_IR37.lfr"
};

ROMDATA INT8U Label_Form38[]={
#include "Label_IR38.lfr"
};

ROMDATA INT8U Label_Form39[]={
#include "Label_IR39.lfr"
};

ROMDATA INT8U Label_Form40[]={
#include "Label_IR40.lfr"
};

ROMDATA INT8U Label_Form41[]={
#include "Label_IR41.lfr"
};

ROMDATA INT8U Label_Form42[]={
#include "Label_IR42.lfr"
};

ROMDATA INT8U Label_Form43[]={
#include "Label_IR43.lfr"
};

ROMDATA INT8U Label_Form44[]={
#include "Label_IR44.lfr"
};

ROMDATA INT8U Label_Form45[]={
#include "Label_IR45.lfr"
};
////////////////////////////////////
#endif  //endif USE_IRN_LABELFORMAT

////////////////////////////////////
// USA LABEL FORMAT
////////////////////////////////////
#ifdef USE_USA_LABELFORMAT
ROMDATA INT8U Label_Form01[]={
#include "Label_US01.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_US02.lfr"
};

ROMDATA INT8U Label_Form03[]={
#include "Label_US03.lfr"
};

ROMDATA INT8U Label_Form04[]={
#include "Label_US04.lfr"
};

ROMDATA INT8U Label_Form05[]={
#include "Label_US05.lfr"
};

ROMDATA INT8U Label_Form06[]={
#include "Label_US06.lfr"
};

ROMDATA INT8U Label_Form07[]={
#include "Label_US07.lfr"
};

ROMDATA INT8U Label_Form08[]={
#include "Label_US08.lfr"
};

ROMDATA INT8U Label_Form09[]={
#include "Label_US09.lfr"
};

ROMDATA INT8U Label_Form10[]={
#include "Label_US10.lfr"
};

ROMDATA INT8U Label_Form11[]={
#include "Label_US11.lfr"
};

ROMDATA INT8U Label_Form12[]={
#include "Label_US12.lfr"
};

ROMDATA INT8U Label_Form13[]={
#include "Label_US13.lfr"
};

ROMDATA INT8U Label_Form14[]={
#include "Label_US14.lfr"
};

ROMDATA INT8U Label_Form15[]={
#include "Label_US15.lfr"
};

ROMDATA INT8U Label_Form16[]={
#include "Label_US16.lfr"
};

ROMDATA INT8U Label_Form17[]={
#include "Label_US17.lfr"
};

ROMDATA INT8U Label_Form18[]={
#include "Label_US18.lfr"
};

ROMDATA INT8U Label_Form19[]={
#include "Label_US19.lfr"
};

ROMDATA INT8U Label_Form20[]={
#include "Label_US20.lfr"
};

ROMDATA INT8U Label_Form21[]={
#include "Label_US21.lfr"
};

ROMDATA INT8U Label_Form22[]={
#include "Label_US22.lfr"
};

ROMDATA INT8U Label_Form23[]={
#include "Label_US23.lfr"
};

ROMDATA INT8U Label_Form24[]={
#include "Label_US24.lfr"
};

ROMDATA INT8U Label_Form25[]={
#include "Label_US25.lfr"
};

ROMDATA INT8U Label_Form26[]={
#include "Label_US26.lfr"
};

ROMDATA INT8U Label_Form27[]={
#include "Label_US27.lfr"
};

ROMDATA INT8U Label_Form28[]={
#include "Label_US28.lfr"
};

ROMDATA INT8U Label_Form29[]={
#include "Label_US29.lfr"
};

ROMDATA INT8U Label_Form30[]={
#include "Label_US30.lfr"
};

ROMDATA INT8U Label_Form31[]={
#include "Label_US31.lfr"
};

ROMDATA INT8U Label_Form32[]={
#include "Label_US32.lfr"
};

ROMDATA INT8U Label_Form33[]={
#include "Label_US33.lfr"
};

ROMDATA INT8U Label_Form34[]={
#include "Label_US34.lfr"
};

ROMDATA INT8U Label_Form35[]={
#include "Label_US35.lfr"
};

ROMDATA INT8U Label_Form36[]={
#include "Label_US36.lfr"
};

ROMDATA INT8U Label_Form37[]={
#include "Label_US37.lfr"
};

ROMDATA INT8U Label_Form38[]={
#include "Label_US38.lfr"
};

ROMDATA INT8U Label_Form39[]={
#include "Label_US39.lfr"
};

ROMDATA INT8U Label_Form40[]={
#include "Label_US40.lfr"
};

ROMDATA INT8U Label_Form41[]={
#include "Label_US41.lfr"
};

ROMDATA INT8U Label_Form42[]={
#include "Label_US42.lfr"
};

ROMDATA INT8U Label_Form43[]={
#include "Label_US43.lfr"
};

ROMDATA INT8U Label_Form44[]={
#include "Label_US44.lfr"
};

ROMDATA INT8U Label_Form45[]={
#include "Label_US45.lfr"
};

ROMDATA INT8U ImageFormSafeHanding[]={
#include "Safe_Handling.lfr"
};
ROMDATA INT8U ImageFormTotalPrice[]={
#include "Total_Price.lfr"
};
ROMDATA INT8U ImageFormSavedPrice[]={
#include "You_Saved_Price.lfr"
};

#endif
////////////////////////////////////

#else//#if !(ROM_COUNTRY == ROM_RUS)
////////////////////////////////////
// RUSSIA LABEL FORMAT
////////////////////////////////////
ROMDATA INT8U Label_Form01[]={
#include "Label_RU01.lfr"
};

ROMDATA INT8U Label_Form02[]={
#include "Label_RU02.lfr"
};

ROMDATA INT8U Label_Form03[]={
#include "Label_RU03.lfr"
};

ROMDATA INT8U Label_Form04[]={
#include "Label_RU04.lfr"
};

//ROMDATA INT8U Label_Form05[]={
//#include "Label_RU05.lfr"
//};
//
//ROMDATA INT8U Label_Form06[]={
//#include "Label_RU06.lfr"
//};
//
//ROMDATA INT8U Label_Form07[]={
//#include "Label_RU07.lfr"
//};
//
//ROMDATA INT8U Label_Form08[]={
//#include "Label_RU08.lfr"
//};
//
//ROMDATA INT8U Label_Form09[]={
//#include "Label_RU09.lfr"
//};
//
//ROMDATA INT8U Label_Form10[]={
//#include "Label_RU10.lfr"
//};

ROMDATA INT8U Label_Form11[]={
#include "Label_RU11.lfr"
};

ROMDATA INT8U Label_Form12[]={
#include "Label_RU12.lfr"
};

ROMDATA INT8U Label_Form13[]={
#include "Label_RU13.lfr"
};

ROMDATA INT8U Label_Form14[]={
#include "Label_RU14.lfr"
};

ROMDATA INT8U Label_Form15[]={
#include "Label_RU15.lfr"
};

ROMDATA INT8U Label_Form16[]={
#include "Label_RU16.lfr"
};

ROMDATA INT8U Label_Form17[]={
#include "Label_RU17.lfr"
};

ROMDATA INT8U Label_Form18[]={
#include "Label_RU18.lfr"
};

ROMDATA INT8U Label_Form19[]={
#include "Label_RU19.lfr"
};

ROMDATA INT8U Label_Form20[]={
#include "Label_RU20.lfr"
};

ROMDATA INT8U Label_Form21[]={
#include "Label_RU21.lfr"
};

ROMDATA INT8U Label_Form22[]={
#include "Label_RU22.lfr"
};

//ROMDATA INT8U Label_Form23[]={
//#include "Label_RU23.lfr"
//};
//
//ROMDATA INT8U Label_Form24[]={
//#include "Label_RU24.lfr"
//};

ROMDATA INT8U Label_Form25[]={
#include "Label_RU25.lfr"
};

ROMDATA INT8U Label_Form26[]={
#include "Label_RU26.lfr"
};

//ROMDATA INT8U Label_Form27[]={
//#include "Label_RU27.lfr"
//};
//
//ROMDATA INT8U Label_Form28[]={
//#include "Label_RU28.lfr"
//};
//
ROMDATA INT8U Label_Form29[]={
#include "Label_RU29.lfr"
};

//ROMDATA INT8U Label_Form30[]={
//#include "Label_RU30.lfr"
//};

ROMDATA INT8U Label_Form31[]={
#include "Label_RU31.lfr"
};

ROMDATA INT8U Label_Form32[]={
#include "Label_RU32.lfr"
};

ROMDATA INT8U Label_Form33[]={
#include "Label_RU33.lfr"
};

ROMDATA INT8U Label_Form34[]={
#include "Label_RU34.lfr"
};

ROMDATA INT8U Label_Form35[]={
#include "Label_RU35.lfr"
};

ROMDATA INT8U Label_Form36[]={
#include "Label_RU36.lfr"
};

ROMDATA INT8U Label_Form37[]={
#include "Label_RU37.lfr"
};

ROMDATA INT8U Label_Form38[]={
#include "Label_RU38.lfr"
};

ROMDATA INT8U Label_Form39[]={
#include "Label_RU39.lfr"
};

ROMDATA INT8U Label_Form40[]={
#include "Label_RU40.lfr"
};

ROMDATA INT8U Label_Form41[]={
#include "Label_RU41.lfr"
};

ROMDATA INT8U Label_Form42[]={
#include "Label_RU42.lfr"
};

//ROMDATA INT8U Label_Form43[]={
//#include "Label_RU43.lfr"
//};
//
//ROMDATA INT8U Label_Form44[]={
//#include "Label_RU44.lfr"
//};
//
//ROMDATA INT8U Label_Form45[]={
//#include "Label_RU45.lfr"
//};
////////////////////////////////////

#endif

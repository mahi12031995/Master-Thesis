// Comm: DueFTS_SP Command Module.
// \license   For educational purposes only. No Warranty. See License.txt
// 28.02.2016 Wolfgang Schulter
// ----------------------------------------------------------------------------

#include "app.h"
#include "dds_fgen.h"   // DDS fgen module
#include "filter.h"		// filter module

#include "cdsm.h"

#include "vars.h"        // variable identifiers for r|w access
#include "ccomm.h"       // nrxbuf, comm_analyze()

extern int16_t dac_offset;    // global offset for the DAC, can be changed with 'ao' command

static char str[512], str2[512];       // helper strings

// ----------------------------------------------------
#ifndef ENABLE_PRINT
// --------------------------------------------------
void Serial_print(char *str) {              
  if (!dsm.bsend)               // halfduplex usage
    Serial.print(str);
};
// --------------------------------------------------
void Serial_println(char *str) {       
  if (!dsm.bsend)               // halfduplex usage
    Serial.println(str);
};
#endif

// ----------------------------------------------------
char *list_tab_int16(int16_t *pstart, int n) {
  char format[8] = "%d%c";    // default format
  char hstr[16];
  str[0] = 0; 
  for (int i=0; i < n; i++) {
    char delim = ' ';
    if (i && !(i % 10)) delim = '\n';
    sprintf(hstr,format, pstart[i],delim);
    strncat(str,hstr,sizeof(str));
  }
  return str;
}

// ----------------------------------------------------
char *list_tab_int32(int32_t *pstart, int n) {
  char format[8] = "%d%c";    // default format
  char hstr[16];
  str[0] = 0; 
  for (int i=0; i < n; i++) {
    char delim = ' ';
    if (i && !(i % 10)) delim = '\n';
    sprintf(hstr,format, pstart[i],delim);
    strncat(str,hstr,sizeof(str));
  }
  return str;
}

// ----------------------------------------------------
char *list_tab_float(float *pstart, int n) {
  char format[8] = "%g%c";    // default format
  char hstr[32];
  str[0] = 0; 
  for (int i=0; i < n; i++) {
    char delim = ' ';
    if (i && !(i % 10)) delim = '\n';
    sprintf(hstr,format, pstart[i],delim);
    strncat(str,hstr,sizeof(str));
  }
  return str;
}

// ------------------------------------
// UART RX String
// ------------------------------------
char rxbuf[NRXBUF];
uint8_t nrxbuf = 0;
// ------------------------------------
char delim[5] = " \t\r\n";

void comm_analyze()
{
  if (!nrxbuf) return;
  // if (do_cyclic_send) do_cyclic_send = 0;  // stop cyclic send

  Serial.print("> ");
  Serial.print(rxbuf);			// cmd prompt
  // dsm.add_str(rxbuf,":\n");	// echo
  if (rxbuf[0] == 'a') {
    //if (do_cyclic_send && !(rxbuf[1]=='d')) do_cyclic_send = 0;  // stop cyclic send
    if (rxbuf[1] == 'i') {
      // ------------------------------------
      // ai command
      // ------------------------------------
      strcpy(str, APPNAME " " APPVERSION " (" APPSHORTID "), " APPTIME);
      Serial.println(str);
      // sprintf(str, "Chip Temp. = %d �C (%u LSB)", get_temp_deg(), temperat);
      // Serial.write(str);
      // ------------------------------------
      // print named DSM signal names
      // ------------------------------------
      sprintf(str, "DSM signals");
      Serial.println(str);
      for (int i = 0; i < NDSM_SIGNALS; i++) {
        sprintf(str, "%d %s", i, dsm.sig_name[i]);
        Serial.println(str);
      }
    
    } else if (rxbuf[1] == 'd') {
      // ------------------------------------
      // ad command
      // ------------------------------------
      dsm.bsend = !dsm.bsend;  
      sprintf(str, "dsm.bsend: %d", dsm.bsend);
      Serial.println(str);

   } else if (rxbuf[1] == 'f') {
      // ------------------------------------
      // af [val] command (app sampling frequency)
      // ------------------------------------
      /*char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t f = atoi(p);
        if ((f >= 500) && (f <= 20000)) {
          //dsm.bsend = 0;
          Timer3.stop();
          fs_Hz = f;
          fgen.set_sampling_freq_hz(fs_Hz);
          Timer3.start(1000000L/fs_Hz);   // FTS period in us
          //dsm.bsend = 1;
        }
      }
      sprintf(str, "sampling frequency %d Hz\n", fs_Hz);
      Serial.println(str);
	  */
   } else if (rxbuf[1] == 'o') {
      // ------------------------------------
      // ao [val] command (app DAC offset)
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t o = atoi(p);
        dac_offset = o;
      }
      // strcpy_P(str2, fgen_mode_str[fgen.mode]);
      sprintf(str, "DAC offset %d \n", dac_offset); //, str2);
      Serial.println(str);
    }
    
  } else 	if (rxbuf[0] == 'g') {		// frequency generator ...

    if (rxbuf[1] == 'm') {
      // ------------------------------------
      // gm [val] command
      // ------------------------------------
      char *p  = strtok(rxbuf + 2, delim);
      bool b_list_modes = 0;
      if (p) {
        int16_t m = atoi(p);
        if ((m >= 0) && (m < FGEN_NUM_MODES))
          fgen.mode = m;
        p  = strtok(NULL, delim); 
      } else 
        b_list_modes = 1;
      sprintf(str, "FGen mode %d = %s", fgen.mode, fgen.get_mode_str() );
      Serial.println(str);
      if (b_list_modes) {
        Serial.println("Fgen modes:");
        for (int i=0; i < FGEN_NUM_MODES; i++) {
          sprintf(str,"%d (%s)\t",i,fgen_mode_str[i]);
          Serial.println(str);
        }
      }

    } else if (rxbuf[1] == 'f') {
      // ------------------------------------
      // gf [val] command
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        float f = atof(p);
        fgen.frq = f * PARAM_NF;
        p  = strtok(NULL, delim); 
      }
      sprintf(str, "FGen freq %.3f Hz\n", (float)fgen.frq/PARAM_NF);
      Serial.println(str);

    } else if (rxbuf[1] == 'a') {
      // ------------------------------------
      // ga [val] command
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t a = atoi(p);
        fgen.amp = a;
        p  = strtok(NULL, delim); 
      }
      sprintf(str, "FGen amp %d\n", fgen.amp);
      Serial.println(str);

    } else if (rxbuf[1] == 'o') {
      // ------------------------------------
      // go [val] command
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t o = atoi(p);
        fgen.offset = o;
        p  = strtok(NULL, delim); 
      }
      sprintf(str, "FGen offset %d\n", fgen.offset);
      Serial.println(str);

    } else if (rxbuf[1] == 'r') {
      // ------------------------------------
      // gr [val] command
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t r = atoi(p);
        fgen.add_rndn_ampl = r;
        p  = strtok(NULL, delim); 
      }
      sprintf(str, "FGen noise amplitde %d\n", fgen.add_rndn_ampl);
      Serial.println(str);
    }

  } else 	if (rxbuf[0] == 'f') {		// filter ...

    if (rxbuf[1]=='m') {
      // ------------------------------------
      // fm [val] command:  filter mode
      // ------------------------------------
      char *p = strtok(rxbuf+2,delim);
      bool b_list_modes = 0;
      if (p) {
      	uint8_t m = atoi(p);
    	if ((m >= 0) && (m < FILTER_NUM_MODES))
    	  filter.mode = m;
      } else 
        b_list_modes = 1;
      sprintf(str, "Filter mode %d (%s), '%s', Mat.:%d", 
        filter.mode, filter.mode_name[filter.mode],init_filter_name,MATR_NR); 
      Serial.println(str);
      if (b_list_modes) {
        Serial.println("Filter modes:");
        for (int i=0; i < FILTER_NUM_MODES; i++) {
          sprintf(str,"%d (%s)\t",i,filter.mode_name[i]);
          Serial.println(str);
        }
      }
      
    } else if (rxbuf[1] == 'd') {
      // ------------------------------------
      // fd command:  filter direct form 1 or 2, quant.mode (0,1,2)
      // ------------------------------------
      char *p = strtok(rxbuf+2,delim);
	  const char *quant_modes[3] = {"round", "floor", "fix"};
      bool b_list_modes = 0;
      if (p) {
      	uint8_t d = atoi(p);
    	if ((d > 0) && (d < 3))
    	  filter.direct_form = d;
	    p  = strtok(NULL, delim); 
		if (p) {
	      uint8_t q = atoi(p);
      	  if ((q >= 0) && (q <= 3))
    	    filter.quant_mode = q;
		}
	  } 
	  sprintf(str,"filter direct form %d, quant.mode = %d (%s)", 
		  filter.direct_form, filter.quant_mode, quant_modes[filter.quant_mode]);
	  Serial.println(str);

    } else if (rxbuf[1] == 'z') {
      // ------------------------------------
      // fz command:  filter zero y buffers
      // ------------------------------------
      filter.init();
	  Serial.println("Zero filter y buffers");

	} else if (rxbuf[1] == 'i') {
      // ------------------------------------
      // fi [val] command:  filter input mode
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      if (p) {
        int16_t m = atoi(p);
        if ((m >= 0) && (m < 2)) filter.input_mode = m;
      }
      sprintf(str, "Filter input mode %d (0 = FGen, 1 = ADC)\n", filter.input_mode);
      Serial.println(str);

    } else if (rxbuf[1] == 'p') {
      // ------------------------------------
      // fp [ind] [val] command:  list filter parameters, change par[ind] --> val
      // ------------------------------------
      char *p = strtok(rxbuf + 2, delim);
      int ind = -1;
      double val;
      if (p) {
        int _ind = atoi(p);
        p = strtok(NULL, delim);
        if (p) {
          val = atof(p);
          ind = _ind;
        }
      }
      switch (filter.mode) {
        case 0:
          if (ind >= 0) filter.my_filter_param = val;
          sprintf(str,"mode %d (%s): my_filter_param = %g", 
            filter.mode, filter.mode_name[filter.mode], filter.my_filter_param);
          Serial.println(str);
          break;
          
        case 1:
          sprintf(str,"Filter param's, mode %d (%s): Nb = %d, Na = %d\nb[] =", 
            filter.mode, filter.mode_name[filter.mode], filter.ciirf.Nb, filter.ciirf.Na);
          Serial.println(str);
          if ((ind >=0) && (ind < filter.ciirf.Nb)) filter.ciirf.b[ind] = val;  // change !
          if ((ind >=filter.ciirf.Nb) && (ind < (filter.ciirf.Nb+filter.ciirf.Na))) 
            filter.ciirf.a[ind-filter.ciirf.Nb] = val;  // change !
          Serial.println(list_tab_float(filter.ciirf.b, filter.ciirf.Nb));
          Serial.println("a[]=");
          Serial.println(list_tab_float(filter.ciirf.a, filter.ciirf.Na));
          break;
          
        case 2:
          sprintf(str,"Filter param's, mode %d (%s): Nb = %d, Na = %d, LDSIIR = %d\nSb[] =", 
            filter.mode, filter.mode_name[filter.mode], filter.ciiri.Nb, filter.ciiri.Na, LDSIIR);
          Serial.println(str);
          if ((ind >=0) && (ind < filter.ciirf.Nb)) filter.ciirf.b[ind] = val;  // change !
          if ((ind >=filter.ciirf.Nb) && (ind < (filter.ciirf.Nb+filter.ciirf.Na))) 
            filter.ciirf.a[ind-filter.ciirf.Nb] = val;  // change !
          Serial.println(list_tab_int32(filter.ciiri.Sb, filter.ciiri.Nb));
          Serial.println("Sa[]=");
          Serial.println(list_tab_int32(filter.ciiri.Sa, filter.ciiri.Na));
          break;

        case 3:
          sprintf(str,"mode %d (%s): M = %d\n", 
            filter.mode, filter.mode_name[filter.mode], filter.cfirf.M);
          Serial.print(str);
          if ((ind >=0) && (ind < filter.cfirf.M)) filter.cfirf.b[ind] = val;  // change !
          Serial.println(list_tab_float((float *)&filter.cfirf.b[0], filter.cfirf.M));
          break;
        
        case 4:
          sprintf(str,"mode %d (%s): M = %d, LDSFIR = %d\n", 
            filter.mode, filter.mode_name[filter.mode], filter.cfiri.M, LDSFIR);
          Serial.print(str);
          if ((ind >=0) && (ind < filter.cfiri.M)) filter.cfiri.Sb[ind] = val;  // change !
          Serial.println(list_tab_int16((int16_t *)&filter.cfiri.Sb[0], filter.cfiri.M));
          break;

        case 5:
          sprintf(str,"mode %d (%s): Nsos = %d\n", 
            filter.mode, filter.mode_name[filter.mode], filter.csosf.Nsos);
          Serial.print(str);
          for (int i=0; i < filter.csosf.Nsos; i++) 
            Serial.println(list_tab_float((float *)&filter.csosf.iir2f[i].b[0], 6));
          break;
        
        case 6:
          sprintf(str,"mode %d (%s): Nsos = %d, ldS = %d\n", 
            filter.mode, filter.mode_name[filter.mode], filter.csosi.Nsos, LDSIIR2);
          Serial.print(str);
          for (int i=0; i < filter.csosi.Nsos; i++)
            Serial.println(list_tab_int32((int32_t *)&filter.csosi.iir2i[i].Sb[0], 6));
          break;

        default:
          Serial.println("unknown");
          str[0] = 0;
      }

    } else if (rxbuf[1]=='q') {
      // ------------------------------------
      // fq [val] command
      // ------------------------------------
      float cpi180 = 180.0/M_PI;
	  float mag = sqrt((float)filter.ciq.ycos2*filter.ciq.ycos2+(float)filter.ciq.ysin2*filter.ciq.ysin2);
      sprintf(str, "IQdet(y) %d, %d --> mag = %.2f (%.2f dB), pha = %.1f deg",
        filter.ciq.ycos2, filter.ciq.ysin2, mag, 20*log10(mag), 
        cpi180*atan2((float)filter.ciq.ycos2,(float)filter.ciq.ysin2));
      Serial.println(str);
    }

  } else if (rxbuf[0] == 'r') {
    // ------------------------------------
    // re - reset
    // ------------------------------------
    if (rxbuf[1] == 'e') {
      Serial.println("soft reset !");
      /*for (long i = 0; i < 900000000L; i++) {
        extern uint16_t FTS_count;
        FTS_count += 1;
      }*/
      asm volatile ("  b 0");	// soft reset
    }
    // ------------------------------------
    // r <var ID> <num>
    // ------------------------------------
    char *p = strtok(rxbuf + 2, delim);
    //long addr = 0;
	TSVAR *pv;
    int n = 1;
	int noff; //, nbytes;
	//if (p) addr = (long)get_var(p, &noff, &nbytes); // addr = strtol(p, 0, 16);
	if (p) pv = get_var(p, &noff); // read|writte by identifier
	if (pv) {	//(addr) {
      p = strtok(0, delim);
      if (p) n = atoi(p);
      //sprintf(str,"%c%c 0x%lx (%d): ",rxbuf[0],rxbuf[1],addr,n);
      // Serial.println(str);
      str[0] = 0;
      for (int i = noff; i < (n+noff); i++) {
		if (i >= pv->nele) break;
		if (pv->type == 'i') {
          int32_t *pi32 = ((int32_t*)pv->addr) + i;
          sprintf(str2, "%ld ", *pi32);
        } else if (pv->type == 'I') {
          uint32_t *pu32 = ((uint32_t*)pv->addr) + i;
          sprintf(str2, "%lu ", *pu32);
        } else if (pv->type == 's') {
          int16_t *pi16 = ((int16_t*)pv->addr) + i;
          sprintf(str2, "%d " , *pi16);
        } else if (pv->type == 'S') {
          uint16_t *pu16 = ((uint16_t*)pv->addr) + i;
          sprintf(str2, "%u " , *pu16);
        } else if (pv->type == 'x') {
          uint8_t *pu8 = ((uint8_t*)pv->addr) + i;
          sprintf(str2, "%02x " , *pu8);
        } else if (pv->type == 'f') {
          float *pf = ((float*)pv->addr) + i;
          sprintf(str2, "%.10g " , *pf);
        } else {
          uint8_t *pu8 = ((uint8_t*)pv->addr) + i;
          sprintf(str2, "%d " , *pu8);
        }
        Serial.print(str2); //strcat(str, str2);
      }
    } else {
      Serial.print(str); //sprintf(str, "%s: error!" , rxbuf);
	  str[0] = 0;
    }
    Serial.println(str);

  } else if (rxbuf[0] == 'w') {
    // ------------------------------------
    // w[x,b,s,i] <hex-addr> <num> commands
    // ------------------------------------
    char *p = strtok(rxbuf + 2, delim);
    //long addr = 0;
	TSVAR *pv;
    long val;
	int noff, nbytes,nitems=0;
	//if (p) addr = (long)get_var(p, &noff, &nbytes); // addr = strtol(p, 0, 16);
	if (p) pv = get_var(p, &noff); // read|writte by identifier
	if (pv) {	//if (addr) {
      //sprintf(str, "%c%c %s: " , rxbuf[0], rxbuf[1], p);
      //Serial.print(str); // str[0] = 0;
	  for (int i = noff; i < (noff+pv->nele); i++, nitems++) { // up to pv->nele arguments for write
        p = strtok(0, delim);
		if (!p) break;
        /*if (!p) {
          sprintf(str2, ". %d items written.\n" , nitems);
          Serial.print(str2); //strcat(str, str2);
          break;
        }*/
  	    if (i >= pv->nele) break;
        val = strtol(p, 0, 0);
        if (pv->type == 'i') {
          int32_t *pi32 = ((int32_t*)pv->addr) + i;
          sprintf(str2, "%ld " , *pi32 = val);
        } else if (pv->type == 'I') {
          uint32_t *pu32 = ((uint32_t*)pv->addr) + i;
          sprintf(str2, "%lu " , *pu32 = val);
        } else if (pv->type == 's') {
          int16_t *pi16 = ((int16_t*)pv->addr) + i;
          sprintf(str2, "%d " , *pi16 = val);
        } else if (pv->type == 'S') {
          uint16_t *pu16 = ((uint16_t*)pv->addr) + i;
          sprintf(str2, "%u " , *pu16 = val);
        } else if (pv->type == 'x') {
          uint8_t *pu8 = ((uint8_t*)pv->addr) + i;
          sprintf(str2, "%02x " , *pu8 = val);
        } else if (pv->type == 'f') {
          float *pf = ((float*)pv->addr) + i;
          sprintf(str2, "%.10g " , *pf = atof(p));
        } else {
          uint8_t *pu8 = ((uint8_t*)pv->addr) + i;
          sprintf(str2, "%d " , *pu8 = val);
        }
        Serial.print(str2); //strcat(str, str2);
      }
      sprintf(str2, ". %d items written.\n" , nitems);
      Serial.print(str2); //strcat(str, str2);
    } else {
      Serial.print(str); //sprintf(str, "%s: error!" , rxbuf);
	  str[0] = 0;
    }
    //Serial.println(str);
  }
}

// ----------------------------------
// the Comm dgb_tic: low prio exec
// ----------------------------------
void comm_tic()
{
  char c;
  while (Serial.available() > 0) {
    c = Serial.read();
    rxbuf[nrxbuf++] = c;
    if (c == '\r') c = '\n';  // Windows -> *NIX termination
    if ((c == '\n') || (nrxbuf == (NRXBUF-1))) {
      /*rxbuf[nrxbuf-1] = */
      rxbuf[nrxbuf] = 0; // term
      comm_analyze();
      nrxbuf = 0;
      rxbuf[0] = 0;
    }
  }
}

/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Nils Hamel <n.hamel@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */


/* 
    Source - Includes
 */

    # include "csps-omvg-frustum.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csList[256] = { 0 };
        char csPair[256] = { 0 };
        char csMntp[256] = "/data/";

        /* Stream pointer variables */
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Camera designation variables */
        char csCamera[256] = { 0 };

        /* Timestamp delay variables */
        long csDelay = 0;

        /* Sensor size variables */
        double csNear =  1.0;
        double csFar  = 30.0;

        /* Size variables */
        unsigned long csSize  = 0;
        unsigned long csAccum = 0;

        /* Pair composition variables */
        unsigned long csaLoop = 0;
        unsigned long csbLoop = 0;

        /* Pair storage buffer */
        unsigned long * csBuffer = NULL;

        /* Stack variables */
        cs_List_t * csStack = NULL;

        /* Calibration data descriptor */
        lf_Descriptor_t lfDesc;

        /* Output stream variables */
        FILE * csStream = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"       , "-p" ), argv,   csPath  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--list"       , "-l" ), argv,   csList  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--pairs"      , "-r" ), argv,   csPair  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--mount-point", "-m" ), argv,   csMntp  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--camera"     , "-c" ), argv,   csCamera, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"    , "-n" ), argv,   csGPSm  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod"    , "-s" ), argv,   csIMUm  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"    , "-g" ), argv,   csGPSd  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag"    , "-i" ), argv,   csIMUd  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--plane-near" , "-e" ), argv, & csNear  , LC_DOUBLE );
        lc_stdp( lc_stda( argc, argv, "--plane-far"  , "-f" ), argv, & csFar   , LC_DOUBLE );
        lc_stdp( lc_stda( argc, argv, "--delay"      , "-d" ), argv, & csDelay , LC_ULONG  );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Create calibration data descriptor */
            if ( lf_parse( ( lf_Char_t * ) csCamera, ( lf_Char_t * ) csMntp, & lfDesc ) == LF_TRUE ) {

                /* Import OpenMVG list */
                if ( ( csSize = cs_omvg_frusmtum_list( csList, & csStack, csDelay ) ) != 0 ) {

                    /* Allocate pair buffer memory */
                    if ( ( csBuffer = ( unsigned long * ) malloc( csSize * sizeof( unsigned long ) ) ) != NULL ) {

                        /* Create output stream handle */
                        if ( ( csStream = fopen( csPair, "w" ) ) != NULL ) {

                            /* CSPS query structures */
                            lp_Query_Position_t    csaQposit;
                            lp_Query_Position_t    csbQposit;
                            lp_Query_Orientation_t csaQorien;
                            lp_Query_Orientation_t csbQorien;

                            /* First level composition loop */
                            for ( csaLoop = 0; csaLoop < csSize; csaLoop ++ ) {

                                /* CSPS query - Positions */
                                csaQposit = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csStack[csaLoop].lsTime );

                                /* CSPS query - Orientations */
                                csaQorien = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csStack[csaLoop].lsTime );

                                /* Second level composition loop */
                                for ( csbLoop = csaLoop + 1; csbLoop < csSize; csbLoop ++ ) {

                                    /* CSPS query - Positions */
                                    csbQposit = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csStack[csbLoop].lsTime );

                                    /* CSPS query - Orientations */
                                    csbQorien = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csStack[csbLoop].lsTime );

                                    /* Check query status */
                                    if ( 

                                        ( csaQposit.qrStatus == LP_TRUE ) && 
                                        ( csbQposit.qrStatus == LP_TRUE ) && 
                                        ( csaQorien.qrStatus == LP_TRUE ) && 
                                        ( csbQorien.qrStatus == LP_TRUE ) 

                                    ) {

                                        /* Frustums variables */
                                        cs_Frustum_t csFrustA, csFrustB;

                                        /* Compute corrected positions - Local flat earth model */
                                        csbQposit.qrLongitude = ( csbQposit.qrLongitude - csaQposit.qrLongitude ) * ( ( ( 6367514.5 + csaQposit.qrAltitude ) * LF_PI / 180.0 ) );
                                        csbQposit.qrLatitude  = ( csbQposit.qrLatitude  - csaQposit.qrLatitude  ) * ( ( ( 6367514.5 + csaQposit.qrAltitude ) * LF_PI / 180.0 ) );
                                        csbQposit.qrAltitude  = ( csbQposit.qrAltitude  - csaQposit.qrAltitude  );

                                        /* Compute frustum of first camera */
                                        cs_omvg_frustum_eyesis4pi( 

                                            csCamera, csStack[csaLoop].lsChannel,
                                            csaQorien.qrfxx, csaQorien.qrfxy, csaQorien.qrfxz,
                                            csaQorien.qrfyx, csaQorien.qrfyy, csaQorien.qrfyz,
                                            csaQorien.qrfzx, csaQorien.qrfzy, csaQorien.qrfzz,
                                            0.0, 0.0, 0.0,
                                            csNear, csFar, & csFrustA, & lfDesc

                                        );

                                        /* Compute frustum of second camera */
                                        cs_omvg_frustum_eyesis4pi( 

                                            csCamera, csStack[csbLoop].lsChannel,
                                            csbQorien.qrfxx, csbQorien.qrfxy, csbQorien.qrfxz,
                                            csbQorien.qrfyx, csbQorien.qrfyy, csbQorien.qrfyz,
                                            csbQorien.qrfzx, csbQorien.qrfzy, csbQorien.qrfzz,
                                            csbQposit.qrLongitude, csbQposit.qrLatitude, csbQposit.qrAltitude,
                                            csNear, csFar, & csFrustB, & lfDesc

                                        );

                                        /* Frustum intersection detection */
                                        if ( cs_omvg_frustum_intersection( & csFrustA, & csFrustB ) == LC_TRUE ) csBuffer[csAccum++] = csbLoop;

                                    /* Display message */
                                    } else { fprintf( LC_ERR, "Warning : unable to query position/orientation with couple (%lu,%lu)\n", csaLoop, csbLoop ); }

                                }

                                /* Check pair buffer state */
                                if ( csAccum != 0 ) {

                                    /* Export reference image index */
                                    fprintf( csStream, "%lu ", csaLoop );

                                    /* Export pair buffer content */
                                    for ( csbLoop = 0; csbLoop < csAccum; csbLoop ++ ) fprintf( csStream, "%lu ", csBuffer[csbLoop] );

                                    /* Send eol to output stream */
                                    fprintf( csStream, "\n" );

                                    /* Reset pair buffer */
                                    csAccum = 0;

                                }

                            }

                        /* Display message */
                        } else { fprintf( LC_ERR, "Error : unable to open output file\n" ); }

                        /* Unallocate pair buffer memory */
                        free( csBuffer );

                    /* Display message */
                    } else { fprintf( LC_ERR, "Error : unable to allocate pair buffer memory\n" ); }

                    /* Unallocate stack */
                    cs_omvg_frusmtum_list( "", & csStack, 0 );

                /* Display message */
                } else { fprintf( LC_ERR, "Error : unable to read OpenMVG list\n" ); }

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to create calibration data descriptor\n" ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - OpenMVG list importation
*/

    unsigned long cs_omvg_frusmtum_list ( 

        char      const *  const csList, 
        cs_List_t       **       csStack, 
        long      const          csDelay 

    ) {

        /* Reading buffer variables */
        char csBuffer[1024] = { 0 };

        /* Stack size variables */
        unsigned long csVirt = 1024;
        unsigned long csSize = 0;

        /* Timestamp composition variables */
        lp_Time_t csSec = 0;
        lp_Time_t csMic = 0;

        /* List handle variables */
        FILE * csStream = NULL;

        /* Check handle creation */
        if ( ( csStream = fopen( csList, "r" ) ) != NULL ) {

            /* Allocate stack initial segment */
            ( * csStack ) = ( cs_List_t * ) malloc( csVirt * sizeof( cs_List_t ) );

            /* Read list by line */
            while ( fgets( csBuffer, sizeof( csBuffer ), csStream ) > 0 ) {

                /* Decompose image name */
                sscanf( csBuffer, "%" lp_Time_i "_%" lp_Time_i "-%lu", & csSec, & csMic, & ( ( ( * csStack ) + csSize )->lsChannel ) );

                /* Compose timestamp */
                ( ( * csStack ) + csSize )->lsTime = lp_timestamp_compose( csSec + csDelay, csMic );

                /* Stack memory management */
                if ( ( ++ csSize ) >= csVirt ) ( * csStack ) = ( cs_List_t * ) realloc( ( * csStack ), ( csVirt += 1024 ) * sizeof( cs_List_t ) );

            }

            /* Close list handle */
            fclose( csStream );

        } else {

            /* Unallocate stack memory */
            if ( ( * csStack ) != NULL ) free( * csStack );

        }

        /* Return stack size */
        return( csSize );

    }

/*
    Source - Eyesis4Pi frustum composer
*/

    void cs_omvg_frustum_eyesis4pi(

        char            const * const csCamera, 
        int             const         csChannel, 
        double          const         csEFxx,
        double          const         csEFxy,
        double          const         csEFxz,
        double          const         csEFyx,
        double          const         csEFyy,
        double          const         csEFyz,
        double          const         csEFzx,
        double          const         csEFzy,
        double          const         csEFzz,
        double          const         csEFpx,
        double          const         csEFpy,
        double          const         csEFpz,
        double          const         csNear,
        double          const         csFar,
        cs_Frustum_t          * const csFrustum,
        lf_Descriptor_t const * const lfDesc

    ) {

        /* Eyesis4pi frame variables */
        double csEyesisframe[3] = { 0.0, 0.0, 0.0 };

        /* Eyesis4pi device calibration variables */
        double csAzimuth   = - lf_query_azimuth              ( csChannel, lfDesc );
        double csHeading   = - lf_query_heading              ( csChannel, lfDesc );
        double csElevation = - lf_query_elevation            ( csChannel, lfDesc );
        double csRoll      = - lf_query_roll                 ( csChannel, lfDesc );
        double csRadius    = + lf_query_radius               ( csChannel, lfDesc );
        double csHeight    = + lf_query_height               ( csChannel, lfDesc );
        double csPupil     = - lf_query_entrancePupilForward ( csChannel, lfDesc );        
        double csPixel     = + lf_query_pixelSize            ( csChannel, lfDesc );
        double csFocal     = + lf_query_focalLength          ( csChannel, lfDesc );
        double csSenWidth  = + lf_query_pixelCorrectionWidth ( csChannel, lfDesc );
        double csSenHeight = + lf_query_pixelCorrectionHeight( csChannel, lfDesc );

        /* Compute corrected azimuth angle */
        csAzimuth = csAzimuth + csHeading + ( LF_PI / 2.0 );

        /* Compute frustum left/top appertures */
        csFrustum->fsLefApp = ( ( csSenWidth  / 2.0 ) * csPixel ) / csFocal;
        csFrustum->fsTopApp = ( ( csSenHeight / 2.0 ) * csPixel ) / csFocal;

        /* Compute frustum near/far planes */
        csFrustum->fsNear = csNear;
        csFrustum->fsFar  = csFar;

        /* Compute frustum nadir-vector in eyesis frame */
        csEyesisframe[0] = + cos( csElevation ) * cos( csAzimuth );
        csEyesisframe[1] = + cos( csElevation ) * sin( csAzimuth );
        csEyesisframe[2] = - sin( csElevation );

        /* Compute frustum nadir-vector in earth frame */
        csFrustum->fsNad[0] = csEyesisframe[0] * csEFxx + csEyesisframe[1] * csEFyx + csEyesisframe[2] * csEFzx;
        csFrustum->fsNad[1] = csEyesisframe[0] * csEFxy + csEyesisframe[1] * csEFyy + csEyesisframe[2] * csEFzy;
        csFrustum->fsNad[2] = csEyesisframe[0] * csEFxz + csEyesisframe[1] * csEFyz + csEyesisframe[2] * csEFzz;

        /* Compute frustum left-vector in eyesis frame */
        csEyesisframe[0] = + sin( csElevation ) * sin( csRoll ) * cos( csAzimuth ) - cos( csRoll ) * sin( csAzimuth );
        csEyesisframe[1] = + sin( csElevation ) * sin( csRoll ) * sin( csAzimuth ) + cos( csRoll ) * cos( csAzimuth );
        csEyesisframe[2] = + cos( csElevation ) * sin( csRoll );

        /* Compute frustum nadir-vector in earth frame */
        csFrustum->fsLef[0] = csEyesisframe[0] * csEFxx + csEyesisframe[1] * csEFyx + csEyesisframe[2] * csEFzx;
        csFrustum->fsLef[1] = csEyesisframe[0] * csEFxy + csEyesisframe[1] * csEFyy + csEyesisframe[2] * csEFzy;
        csFrustum->fsLef[2] = csEyesisframe[0] * csEFxz + csEyesisframe[1] * csEFyz + csEyesisframe[2] * csEFzz;

        /* Compute frustum top-vector in earth frame */
        csFrustum->fsTop[0] = csFrustum->fsNad[1] * csFrustum->fsLef[2] - csFrustum->fsNad[2] * csFrustum->fsLef[1];
        csFrustum->fsTop[1] = csFrustum->fsNad[2] * csFrustum->fsLef[0] - csFrustum->fsNad[0] * csFrustum->fsLef[2];
        csFrustum->fsTop[2] = csFrustum->fsNad[0] * csFrustum->fsLef[1] - csFrustum->fsNad[1] * csFrustum->fsLef[0];

        /* Compute frustum origin point in earth frame */
        csFrustum->fsOrg[0] = + csEFpx + ( csRadius - csPupil ) * cos( csAzimuth );
        csFrustum->fsOrg[1] = + csEFpy + ( csRadius - csPupil ) * sin( csAzimuth );
        csFrustum->fsOrg[2] = + csEFpz + ( sin( csElevation ) * csPupil ) + csHeight;

        /* Compute frustum near-principal point in earth frame */
        csFrustum->fsNPP[0] = csFrustum->fsOrg[0] + csFrustum->fsNad[0] * csFrustum->fsNear;
        csFrustum->fsNPP[1] = csFrustum->fsOrg[1] + csFrustum->fsNad[1] * csFrustum->fsNear;
        csFrustum->fsNPP[2] = csFrustum->fsOrg[2] + csFrustum->fsNad[2] * csFrustum->fsNear;

        /* Compute frustum far-principal point in earth frame */
        csFrustum->fsFPP[0] = csFrustum->fsOrg[0] + csFrustum->fsNad[0] * csFrustum->fsFar;
        csFrustum->fsFPP[1] = csFrustum->fsOrg[1] + csFrustum->fsNad[1] * csFrustum->fsFar;
        csFrustum->fsFPP[2] = csFrustum->fsOrg[2] + csFrustum->fsNad[2] * csFrustum->fsFar;

        /* Compute frustum summits in earth frame x-near-coordinates */
        csFrustum->fsSX[0] = csFrustum->fsNPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[1] = csFrustum->fsNPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[2] = csFrustum->fsNPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[3] = csFrustum->fsNPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame y-near-coordinates */
        csFrustum->fsSY[0] = csFrustum->fsNPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[1] = csFrustum->fsNPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[2] = csFrustum->fsNPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[3] = csFrustum->fsNPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame z-near-coordinates */
        csFrustum->fsSZ[0] = csFrustum->fsNPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[1] = csFrustum->fsNPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[2] = csFrustum->fsNPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[3] = csFrustum->fsNPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame x-far-coordinates */
        csFrustum->fsSX[4] = csFrustum->fsFPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[5] = csFrustum->fsFPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[6] = csFrustum->fsFPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[7] = csFrustum->fsFPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );

        /* Compute frustum summits in earth frame y-far-coordinates */
        csFrustum->fsSY[4] = csFrustum->fsFPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[5] = csFrustum->fsFPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[6] = csFrustum->fsFPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[7] = csFrustum->fsFPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );

        /* Compute frustum summits in earth frame z-far-coordinates */
        csFrustum->fsSZ[4] = csFrustum->fsFPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[5] = csFrustum->fsFPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[6] = csFrustum->fsFPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[7] = csFrustum->fsFPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );

    }

/*
    Source - Frustum intersection detection
 */

    int cs_omvg_frustum_intersection(

        cs_Frustum_t const * const csFrus_A,
        cs_Frustum_t const * const csFrus_B

    ) {

        /* Frustum segment index variables */
        static int csEdge[12][2] = { { 1, 0 }, { 2, 1 }, { 3, 2 }, { 0, 3 }, { 5, 4 }, { 6, 5 }, { 7, 6 }, { 4, 7 }, { 4, 0 }, { 5, 1 }, { 6, 2 }, { 7, 3 } };

        /* Secondary loop variables */
        int csParse = 0;

        /* Edge point coordinates variables */
        double csInterX = 0.0;
        double csInterY = 0.0;
        double csInterZ = 0.0;

        /* Edge point component variables */
        double csCompX = 0.0;
        double csCompY = 0.0;
        double csCompZ = 0.0;

        /* Progressive rational range variables */
        double csStp = 1.0;
        double csPow = 2.0;
        double csPtn = 1.0 / 2.0;

        /* Return variables */
        int csResult = LC_FALSE;

        /* Progressive edge inclusion detection */
        while ( ( csPow < 64 ) && ( csResult == LC_FALSE ) ) {

            /* Secondary loop on polyhedron edges */
            for ( csParse = 0; csParse < 12; csParse ++ ) {

                /* Compute edge point in first frustum origin centered frame */
                csInterX = csPtn * ( csFrus_B->fsSX[csEdge[csParse][0]] - csFrus_B->fsSX[csEdge[csParse][1]] ) + csFrus_B->fsSX[csEdge[csParse][1]] - csFrus_A->fsOrg[0];
                csInterY = csPtn * ( csFrus_B->fsSY[csEdge[csParse][0]] - csFrus_B->fsSY[csEdge[csParse][1]] ) + csFrus_B->fsSY[csEdge[csParse][1]] - csFrus_A->fsOrg[1];
                csInterZ = csPtn * ( csFrus_B->fsSZ[csEdge[csParse][0]] - csFrus_B->fsSZ[csEdge[csParse][1]] ) + csFrus_B->fsSZ[csEdge[csParse][1]] - csFrus_A->fsOrg[2];

                /* Compute edge point coordinate in frist frustum nadir/left/top basis */
                csCompX = csFrus_A->fsNad[0] * csInterX + csFrus_A->fsNad[1] * csInterY + csFrus_A->fsNad[2] * csInterZ;
                csCompY = csFrus_A->fsLef[0] * csInterX + csFrus_A->fsLef[1] * csInterY + csFrus_A->fsLef[2] * csInterZ;
                csCompZ = csFrus_A->fsTop[0] * csInterX + csFrus_A->fsTop[1] * csInterY + csFrus_A->fsTop[2] * csInterZ;

                /* Verify intersection condition - principal direction */
                if ( ( csCompX >= csFrus_A->fsNear ) && ( csCompX <= csFrus_A->fsFar ) ) {

                    /* Verify intersection condition - secondary plane */
                    if ( ( fabs( csCompY ) <= ( csCompX * csFrus_A->fsLefApp ) ) && ( fabs( csCompZ ) <= ( csCompX * csFrus_A->fsTopApp ) ) ) {

                        /* Trigger intersection condition */
                        csResult = LC_TRUE;

                    }

                }

            }

            /* Compute next point */
            if ( ( csPtn += csStp ) > 1.0 ) {

                /* Parsing next rational scale */
                csStp = 1.0 / ( csPow );
                csPtn = 1.0 / ( csPow *= 2.0 );

            }

        }

        /* Return intersection detection results */
        return( csResult );

    }

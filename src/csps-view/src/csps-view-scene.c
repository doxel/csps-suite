/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
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

    # include "csps-view-scene.h"

/* 
    Source - General variables (GLUT only)
 */

    extern cs_view_keyboard csKeyboard;

/*
    Source - Scene main function
 */

    void cs_view_scene( void ) {

        /* Enable depth test */
        glEnable( GL_DEPTH_TEST );

        /* Update clear color */
        glClearColor( 1.0, 1.0, 1.0, 0.0 );

        /* Clear buffers */
        glClear( GL_COLOR_BUFFER_BIT );
        glClear( GL_DEPTH_BUFFER_BIT );

        /* Update matrix mode */
        glMatrixMode( GL_MODELVIEW );

        /* Push matrix */
        glPushMatrix();

        /* Reset matrix */
        glLoadIdentity();

        /* ... */

        /* Pop matrix */
        glPopMatrix();

        /* Force primitives draw */
        glFlush();

        /* Wait for primitives draw */
        glFinish();

    }
    
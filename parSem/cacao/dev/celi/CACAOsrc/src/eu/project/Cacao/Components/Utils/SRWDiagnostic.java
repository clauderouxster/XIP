/* 
 * OCKHAM P2PREGISTRY Copyright 2004 Oregon State University 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

package eu.project.Cacao.Components.Utils;

/**
 * Extension of SRWDiagnostic that makes code and addinfo public
 *
 * @author peter
 *         Date: Apr 21, 2006
 *         Time: 12:42:28 PM
 */
public class SRWDiagnostic extends ORG.oclc.os.SRW.SRWDiagnostic {



    public SRWDiagnostic(int code, String addInfo) {
        super(code, addInfo);
        this.code = code;
        this.addInfo = addInfo;
    }

    /**
     *  code
     */
    public int code;

    /**
     *  addInfo
     */
    public String addInfo;
 
}

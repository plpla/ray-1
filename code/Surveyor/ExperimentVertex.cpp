/*
    Copyright 2013 Sébastien Boisvert
    Copyright 2013 Université Laval
    Copyright 2013 Centre Hospitalier Universitaire de Québec

    This file is part of Ray Surveyor.

    Ray Surveyor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Ray Surveyor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ray Surveyor.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ExperimentVertex.h"

ExperimentVertex::ExperimentVertex() {

}

ExperimentVertex::~ExperimentVertex() {

}

Kmer ExperimentVertex::getKey() const {

	return m_kmer;
}

void ExperimentVertex::setKey(Kmer & kmer) {

	m_kmer = kmer;
}

void ExperimentVertex::setVirtualColor(VirtualKmerColorHandle handle) {

	m_color = handle;
}

VirtualKmerColorHandle ExperimentVertex::getVirtualColor() const {

	return m_color;
}

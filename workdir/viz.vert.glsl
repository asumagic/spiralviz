// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
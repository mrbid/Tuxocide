/*
    James William Fletcher  ( notabug.org/Vandarin )
        November 2023       ( github.com/mrbid     )

    a war rages on in antarctica... it's tuxocide.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> // tolower

#ifdef WEB
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#endif

#define uint GLushort
#define sint GLshort

#include "inc/gl.h"
#define GLFW_INCLUDE_NONE
#include "inc/glfw3.h"

#include "inc/esAux4.h"

#include "inc/res.h"
#include "assets/intro.h"
#include "assets/ak47.h"
#include "assets/alien.h"
#include "assets/alien_pelt.h"
#include "assets/beam.h"
#include "assets/bullet.h"
#include "assets/icehole.h"
#include "assets/pelt.h"
#include "assets/terrain.h"
#include "assets/tree.h"
#include "assets/tux.h"
#include "assets/ufo.h"

//*************************************
// globals
//*************************************
const char appTitle[]="Tuxocide";
GLFWwindow* window;
uint winw=1024, winh=768;
float t=0.f, dt=0.f, lt=0.f, st=0.f, fc=0.f, lfct=0.f, aspect;
double mx,my,lx,ly,ww,wh;

// render state id's
GLint projection_id;
GLint modelview_id;
GLint position_id;
GLint normal_id;
GLint color_id;
GLint lightpos_id;
GLint opacity_id;

// render state
mat projection, view, model, modelview;

// models
ESModel mdlIntro;
ESModel mdlAK47;
ESModel mdlAlien;
ESModel mdlAlienPelt;
ESModel mdlBeam;
ESModel mdlBullet;
ESModel mdlIcehole;
ESModel mdlPelt;
ESModel mdlTerrain;
ESModel mdlTree;
ESModel mdlTux;
ESModel mdlUFO;

// game vars
#define FAR_DISTANCE 32.f
#define NEWGAME_SEED 1337
char tts[32];
uint keystate[5] = {0};
int TREE_SEED = 1337;

// camera vars
uint focus_cursor = 0;
double sens = 0.003f;
float xrot = 0.f;
vec look_dir, lookx, looky;

// player vars
float MOVE_SPEED = 0.6f;
vec pp;

// bullets
#define MAX_BULLETS  32
#define BULLET_SPEED 3.6f
vec bullet_pos[MAX_BULLETS];
vec bullet_dir[MAX_BULLETS];
vec barrel_pos;
float akdist = 0.17f;
float akxo = -0.074f;
float akreset = 0.f;

// tuxes
#define MAX_TUX   256
#define TUX_SPEED 0.07f
vec tux_pos[MAX_TUX];
float tux_nextspawn = 0.f;

// aliens
#define MAX_ALIEN   64
#define ALIEN_SPEED 0.14f
vec alien_pos[MAX_ALIEN];
uint alien_pelted[MAX_ALIEN];

// ufo
vec ufo_pos;
vec ufo_spawnpos;
uint ufo_above_pelted_alien = 0;

// stats
uint tuxkilled = 0;
uint alienpelts = 0;
uint alienskilled = 0;

//*************************************
// utility functions
//*************************************
void timestamp(char* ts)
{
    const time_t tt = time(0);
    strftime(ts, 16, "%H:%M:%S", localtime(&tt));
}
void timeTaken(uint ss)
{
    if(ss == 1)
    {
        const double tt = t-st;
        if(tt < 60.0)
            sprintf(tts, "%.0f Sec", tt);
        else if(tt < 3600.0)
            sprintf(tts, "%.2f Min", tt * 0.016666667);
        else if(tt < 216000.0)
            sprintf(tts, "%.2f Hr", tt * 0.000277778);
        else if(tt < 12960000.0)
            sprintf(tts, "%.2f Days", tt * 0.00000463);
    }
    else
    {
        const double tt = t-st;
        if(tt < 60.0)
            sprintf(tts, "%.0f Seconds", tt);
        else if(tt < 3600.0)
            sprintf(tts, "%.2f Minutes", tt * 0.016666667);
        else if(tt < 216000.0)
            sprintf(tts, "%.2f Hours", tt * 0.000277778);
        else if(tt < 12960000.0)
            sprintf(tts, "%.2f Days", tt * 0.00000463);
    }
}

//*************************************
// render functions
//*************************************
void modelBind(const ESModel* mdl)
{
    glBindBuffer(GL_ARRAY_BUFFER, mdl->vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iid);
}
void modelBind1(const ESModel* mdl)
{
    glBindBuffer(GL_ARRAY_BUFFER, mdl->vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);

    glBindBuffer(GL_ARRAY_BUFFER, mdl->nid);
    glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normal_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iid);
}
void modelBind3(const ESModel* mdl)
{
    glBindBuffer(GL_ARRAY_BUFFER, mdl->cid);
    glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(color_id);

    glBindBuffer(GL_ARRAY_BUFFER, mdl->vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);

    glBindBuffer(GL_ARRAY_BUFFER, mdl->nid);
    glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normal_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->iid);
}

//*************************************
// game functions
//*************************************
void newGame(unsigned int seed)
{
    srand(seed);
    TREE_SEED = seed;

    pp = (vec){0.f, 0.f, 0.f};
    xrot = 0.f;
    ufo_pos = (vec){0.f, 6.f, 0.3f};
    ufo_spawnpos = ufo_pos;
    tux_nextspawn = 0.f;
    akdist = 0.17f;
    akreset = 0.f;
    lt = 0.f;

    for(int i = 0; i < MAX_BULLETS; i++){bullet_pos[i].z = -3.f;}
    for(int i = 0; i < MAX_TUX; i++){tux_pos[i].z = -3.f;}
    for(int i = 0; i < MAX_ALIEN; i++)
    {
        alien_pos[i].z = -3.f;
        alien_pelted[i] = 0;
    }

    tuxkilled = 0;
    alienpelts = 0;
    alienskilled = 0;

    char strts[16];
    timestamp(&strts[0]);
    printf("[%s] Game Start [%u].\n", strts, seed);
    
    glfwSetWindowTitle(window, appTitle);
    glfwSetTime(0.0);
}
void updateTitle()
{
    char tmp[256];
    sprintf(tmp, "TUX KILLED %u - PELTS HARVESTED %u - ALIENS KILLED %u", tuxkilled, alienpelts, alienskilled);
    glfwSetWindowTitle(window, tmp);
}

//*************************************
// update & render
//*************************************
void main_loop()
{
//*************************************
// core logic
//*************************************
    fc++;
    glfwPollEvents();
    t = (float)glfwGetTime();
    dt = t-lt;
    lt = t;

#ifdef WEB
    EmscriptenPointerlockChangeEvent e;
    if(emscripten_get_pointerlock_status(&e) == EMSCRIPTEN_RESULT_SUCCESS)
    {
        if(focus_cursor == 0 && e.isActive == 1)
        {
            glfwGetCursorPos(window, &lx, &ly);
        }
        focus_cursor = e.isActive;
    }
#endif

//*************************************
// game logic
//*************************************

    // forward & backward
    if(keystate[2] == 1)
    {
        vec m;
        vMulS(&m, look_dir, MOVE_SPEED * dt);
        vSub(&pp, pp, m);
    }
    else if(keystate[3] == 1)
    {
        vec m;
        vMulS(&m, look_dir, MOVE_SPEED * dt);
        vAdd(&pp, pp, m);
    }

    // strafe left & right
    if(keystate[0] == 1)
    {
        mGetViewX(&lookx, view);
        vec m;
        vMulS(&m, lookx, MOVE_SPEED * dt);
        vSub(&pp, pp, m);
    }
    else if(keystate[1] == 1)
    {
        mGetViewX(&lookx, view);
        vec m;
        vMulS(&m, lookx, MOVE_SPEED * dt);
        vAdd(&pp, pp, m);
    }

    // sprint
    if(keystate[4] == 1)
    {
        MOVE_SPEED = 2.6f;
    }
    else
    {
        MOVE_SPEED = 0.6f;
    }

    // lock to floor
    pp.z = -0.06f;

    // reset ak47 recoil
    if(akreset != 0.f && t > akreset)
    {
        akdist = 0.17f;
        akreset = 0.f;
    }

    // tux spawner
    if(t > tux_nextspawn)
    {
        for(int i = 0; i < MAX_TUX; i++)
        {
            if(tux_pos[i].z == -3.f)
            {
                tux_pos[i].z = -2.99f;
                tux_pos[i].x = esRandFloat(-8.f, 8.f);
                tux_pos[i].y = esRandFloat(-8.f, -6.f);
                float msp = 3.f - (t*0.01f);
                if(msp < 0.1f){msp = 0.1f;}
                tux_nextspawn = t + esRandFloat(0.f, msp);
                break;
            }
        }
    }

    // alien spawner
    if(ufo_pos.x == ufo_spawnpos.x && ufo_pos.y == ufo_spawnpos.y)
    {
        ufo_above_pelted_alien = 0;
        uint fail = 1;
        int pa = -1;
        for(int i = 0; i < MAX_ALIEN; i++)
        {
            if(alien_pelted[i] == 1)
            {
                pa = i;
                break;
            }
            if(alien_pos[i].z == -3.f)
            {
                alien_pos[i].z = 0.f;
                alien_pos[i].x = ufo_pos.x;
                alien_pos[i].y = ufo_pos.y;
                fail = 0;
                break;
            }
        }

        if(pa != -1)
        {
            ufo_spawnpos.x = alien_pos[pa].x;
            ufo_spawnpos.y = alien_pos[pa].y;
        }
        else if(fail == 0)
        {
            ufo_spawnpos.x = esRandFloat(-8.f, 8.f);
            ufo_spawnpos.y = esRandFloat(6.f, 8.f);
        }
    }

    // move ufo
    const float uxi = ufo_spawnpos.x - ufo_pos.x;
    const float uyi = ufo_spawnpos.y - ufo_pos.y;
    if(fabsf(uxi) < 0.01f){ufo_pos.x = ufo_spawnpos.x;}
    if(fabsf(uyi) < 0.01f){ufo_pos.y = ufo_spawnpos.y;}
    ufo_pos.x += uxi*0.01f;
    ufo_pos.y += uyi*0.01f;

//*************************************
// camera
//*************************************
    if(focus_cursor == 1)
    {
        glfwGetCursorPos(window, &mx, &my);
        xrot += (lx-mx)*sens;
        lx = mx;
    }
    mIdent(&view);
    mRotate(&view, 1.5f, 1.f, 0.f, 0.f); // technically its d2PI not 1.5 but whatever
    mRotate(&view, xrot, 0.f, 0.f, 1.f);
    mTranslate(&view, pp.x, pp.y, pp.z);

    // get look dir/axes
    mGetViewZ(&look_dir, view);
    mGetViewX(&lookx, view);
    mGetViewY(&looky, view);

//*************************************
// render
//*************************************

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ///

    // shader program change
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);

    // render terrain
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&view.m[0][0]);
    modelBind(&mdlTerrain);
    glUniform3f(color_id, 1.f, 1.f, 1.f);
    glDrawElements(GL_TRIANGLES, terrain_numind, GL_UNSIGNED_BYTE, 0);

    ///

    // shader program change
    shadeLambert1(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);

    // render bullet
    glUniform3f(color_id, 0.722f, 0.525f, 0.333f);
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet_pos[i].z != -3.f)
        {
            vec inc;
            vMulS(&inc, bullet_dir[i], BULLET_SPEED * dt);
            vAdd(&bullet_pos[i], bullet_pos[i], inc);
            
            if(vMag(bullet_pos[i]) > 333.f){bullet_pos[i].z = -3.f; continue;} // bullet left game bounds

            mIdent(&model);
            
            ///
            const vec up_norm = (vec){0.f, 0.f, 1.f};

            vec c;
            vCross(&c, up_norm, bullet_dir[i]);
            vNorm(&c);

            vec rup;
            vCross(&rup, bullet_dir[i], c);

            model.m[0][0] = -c.x;
            model.m[0][1] = -c.y;
            model.m[0][2] = -c.z;

            model.m[2][0] = rup.x;
            model.m[2][1] = rup.y;
            model.m[2][2] = rup.z;

            model.m[1][0] = bullet_dir[i].x;
            model.m[1][1] = bullet_dir[i].y;
            model.m[1][2] = bullet_dir[i].z;
            ///

            mSetPos(&model, bullet_pos[i]);
            mMul(&modelview, &model, &view);
            glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
            modelBind1(&mdlBullet);
            glDrawElements(GL_TRIANGLES, bullet_numind, GL_UNSIGNED_BYTE, 0);
        }
    }

    ///

    // shader program change
    shadeLambert3(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);

    // render ufo
    mIdent(&model);
    mSetPos(&model, ufo_pos);
    mMul(&modelview, &model, &view);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
    modelBind3(&mdlUFO);
    glDrawElements(GL_TRIANGLES, ufo_numind, GL_UNSIGNED_SHORT, 0);

    // render trees (so funny maybe should make them real collision objects?)
    srandf(TREE_SEED);
    for(int i = 0; i < 1024; i++)
    {
        // render tree
        mIdent(&model);
        mSetPos(&model, (vec){fRandFloat(-8.f, 8.f), fRandFloat(-8.f, 8.f), 0.f});
        mMul(&modelview, &model, &view);
        glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
        modelBind3(&mdlTree);
        glDisable(GL_CULL_FACE);
            glDrawElements(GL_TRIANGLES, tree_numind, GL_UNSIGNED_BYTE, 0);
        glEnable(GL_CULL_FACE);
    }

    // render alien
    for(int i = 0; i < MAX_ALIEN; i++)
    {
        if(alien_pos[i].z != -3.f)
        {
            // die time alien
            if(alien_pos[i].z <= -4.f)
            {
                alien_pos[i].z -= 0.06f * dt;
                if(alien_pos[i].z < -4.12f)
                {
                    alien_pos[i].z = -3.f;
                }
                mIdent(&model);
                mSetPos(&model, (vec){alien_pos[i].x, alien_pos[i].y, alien_pos[i].z+4.f});
                mRotZ(&model, t*7.2f);
                mMul(&modelview, &model, &view);
                glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
                modelBind3(&mdlAlien);
                glDrawElements(GL_TRIANGLES, alien_numind, GL_UNSIGNED_SHORT, 0);
                continue;
            }

            // bullet hit a alien?
            for(int j = 0; j < MAX_BULLETS; j++)
            {
                if(bullet_pos[j].z != -3.f)
                {
                    const float xm = (alien_pos[i].x - bullet_pos[j].x);
                    const float ym = (alien_pos[i].y - bullet_pos[j].y);
                    if(xm*xm + ym*ym < 0.001225f)
                    {
                        alien_pelted[i] = 0;
                        alien_pos[i].z = -4.f;
                        bullet_pos[j].z = -3.f;
                        alienskilled++;
                        updateTitle();
                        break;
                    }
                }
            }

            if(alien_pelted[i] == 1)
            {
                const float xm = (alien_pos[i].x - ufo_pos.x);
                const float ym = (alien_pos[i].y - ufo_pos.y);
                if(xm*xm + ym*ym < 0.06f)
                {
                    ufo_above_pelted_alien = 1;
                    alien_pelted[i] = 0;
                    alienpelts++;
                    updateTitle();
                }
            }

            mIdent(&model);
            mSetPos(&model, alien_pos[i]);

            // find nearest target
            vec tp;
            if(alien_pelted[i] == 0)
            {
                float ld = 1337.f;
                for(int j = 0; j < MAX_TUX; j++)
                {
                    if(tux_pos[j].z != -3.f)
                    {
                        float nd;
                        if(tux_pos[j].z == -4.f)
                        {
                            nd = vDistSq(alien_pos[i], (vec){tux_pos[j].x, tux_pos[j].y, tux_pos[j].z+4.f});
                        }
                        else
                        {
                            nd = vDistSq(alien_pos[i], tux_pos[j]);
                        }
                        if(nd < 0.003)
                        {
                            tux_pos[j].z = -3.f;
                            alien_pelted[i] = 1;
                            tuxkilled++;
                            updateTitle();
                        }
                        if(nd < ld)
                        {
                            ld = nd;
                            tp = tux_pos[j];
                        }
                    }
                }
            }
            else
            {
                tp = ufo_pos;
            }

            vec dir_norm = tp;
            vSub(&dir_norm, dir_norm, alien_pos[i]);
            dir_norm.z = 0.f;
            vNorm(&dir_norm);
            mSetDir(&model, dir_norm, (vec){0.f, 0.f, 1.f});
            
            vec inc;
            vMulS(&inc, dir_norm, ALIEN_SPEED * dt);
            vAdd(&alien_pos[i], alien_pos[i], inc);

            mMul(&modelview, &model, &view);
            glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
            modelBind3(&mdlAlien);
            glDrawElements(GL_TRIANGLES, alien_numind, GL_UNSIGNED_SHORT, 0);

            if(alien_pelted[i] == 1)
            {
                modelBind3(&mdlAlienPelt);
                glDrawElements(GL_TRIANGLES, alien_pelt_numind, GL_UNSIGNED_BYTE, 0);
            }
        }
    }

    // render tux
    for(int i = 0; i < MAX_TUX; i++)
    {
        if(tux_pos[i].z != -3.f)
        {
            if(tux_pos[i].y > 14.f) // tux left game bounds - go back down the icehole
            {
                // sink tux
                tux_pos[i].z -= 0.02f * dt;

                // done
                if(tux_pos[i].z < -0.1f)
                {
                    tux_pos[i].z = -3.f;
                    continue;
                }

                // render icehole
                float opa = 1.f - ((tux_pos[i].z+0.07f) * -33.333333333f);
                if(opa > 1.f){opa = 1.f;}
                glUniform1f(opacity_id, opa);
                mIdent(&model);
                mSetPos(&model, (vec){tux_pos[i].x, tux_pos[i].y, 0.f});
                mMul(&modelview, &model, &view);
                glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
                modelBind3(&mdlIcehole);
                if(opa < 1.f){glEnable(GL_BLEND);}
                    glDrawElements(GL_TRIANGLES, icehole_numind, GL_UNSIGNED_BYTE, 0);
                if(opa < 1.f){glDisable(GL_BLEND);}

                // render tux
                mIdent(&model);
                mSetPos(&model, tux_pos[i]);
                mRotZ(&model, t*7.2f);
                mMul(&modelview, &model, &view);
                glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
                modelBind3(&mdlTux);
                glDrawElements(GL_TRIANGLES, tux_numind, GL_UNSIGNED_SHORT, 0);

                //done
                continue;
            }
            else if(tux_pos[i].z == -4.f) // is pelted? aka dead
            {
                // bullet hit a pelt?
                for(int j = 0; j < MAX_BULLETS; j++)
                {
                    if(bullet_pos[j].z != -3.f)
                    {
                        const float xm = (tux_pos[i].x - bullet_pos[j].x);
                        const float ym = (tux_pos[i].y - bullet_pos[j].y);
                        if(xm*xm + ym*ym < 0.001225f)
                        {
                            tux_pos[i].z = -3.f;
                            bullet_pos[j].z = -3.f;
                            break;
                        }
                    }
                }

                // render pelt
                mIdent(&model);
                mSetPos(&model, (vec){tux_pos[i].x, tux_pos[i].y, 0.f});
                mRotZ(&model, t*1.3f);
                mMul(&modelview, &model, &view);
                glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
                modelBind3(&mdlPelt);
                glDrawElements(GL_TRIANGLES, pelt_numind, GL_UNSIGNED_BYTE, 0);

                // done
                continue;
            }
            else if(tux_pos[i].z < 0.f) // rise from the icehole
            {
                // rise tux
                tux_pos[i].z += 0.1f * dt;

                // render icehole
                float opa = tux_pos[i].z+3.f;
                if(opa > 1.f){opa = 1.f;}
                glUniform1f(opacity_id, opa);
                mIdent(&model);
                mSetPos(&model, (vec){tux_pos[i].x, tux_pos[i].y, 0.f});
                mMul(&modelview, &model, &view);
                glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
                modelBind3(&mdlIcehole);
                if(opa < 1.f){glEnable(GL_BLEND);}
                    glDrawElements(GL_TRIANGLES, icehole_numind, GL_UNSIGNED_BYTE, 0);
                if(opa < 1.f){glDisable(GL_BLEND);}
            }
            else // move tux
            {
                tux_pos[i].y += TUX_SPEED * dt;
            } 

            // bullet hit a tux?
            for(int j = 0; j < MAX_BULLETS; j++)
            {
                if(bullet_pos[j].z != -3.f)
                {
                    const float xm = (tux_pos[i].x - bullet_pos[j].x);
                    const float ym = (tux_pos[i].y - bullet_pos[j].y);
                    if(xm*xm + ym*ym < 0.001225f)
                    {
                        tux_pos[i].z = -4.f;
                        bullet_pos[j].z = -3.f;
                        tuxkilled++;
                        updateTitle();
                        break;
                    }
                }
            }

            // render tux
            mIdent(&model);
            mSetPos(&model, tux_pos[i]);
            mRotZ(&model, 180.f*DEG2RAD); // can re-export to right rotation
            mMul(&modelview, &model, &view);
            glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
            modelBind3(&mdlTux);
            glDrawElements(GL_TRIANGLES, tux_numind, GL_UNSIGNED_SHORT, 0);
        }
    }

    ///
    
    // render beam
    if((fabsf(uxi) < 0.1f && fabsf(uyi) < 0.1f) || ufo_above_pelted_alien != 0)
    {
        // shader program change
        shadeLambert1(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
        glUniform3f(color_id, 0.00784f, 1.f, 0.f);
        glUniform1f(opacity_id, 0.5f);
        mIdent(&model);
        mSetPos(&model, ufo_pos);
        mMul(&modelview, &model, &view);
        glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
        modelBind1(&mdlBeam);
        glEnable(GL_BLEND);
            glDrawElements(GL_TRIANGLES, beam_numind, GL_UNSIGNED_BYTE, 0);
        glDisable(GL_BLEND);
        // shader program change
        shadeLambert3(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    }

    ///

    // render AK-47
    mIdent(&model);
    vec ld = look_dir;
    ld.z = 0.f;
    if(akxo == 0.f)
    {
        vMulS(&ld, ld, akdist-0.08f); // far from camera
    }
    else
    {
        vMulS(&ld, ld, akdist); // far from camera
    }
    vec np = (vec){-pp.x, -pp.y, -pp.z};
    vAdd(&np, np, ld);

    // x offset
    vec vx = lookx;
    vMulS(&vx, vx, akxo);
    vAdd(&np, np, vx);

    // y offset
    vec vy = looky;
    if(akxo == 0.f)
    {
        vMulS(&vy, vy, 0.0088f);
    }
    else
    {
        vMulS(&vy, vy, 0.034f);
    }
    vAdd(&np, np, vy);
    barrel_pos = np;

    // rotate to camera direction with slight offset and set pos
    if(akxo == 0.f)
    {
        mRotZ(&model, -(xrot+0.001f));
    }
    else
    {
        mRotZ(&model, -(xrot-0.28f));
    }
    mSetPos(&model, np);

    // render it
    mMul(&modelview, &model, &view);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
    modelBind3(&mdlAK47);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, ak47_numind, GL_UNSIGNED_SHORT, 0);

    ///

    // render intro
    if(t < 5.f)
    {
        // shader program change
        shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);

        if(t < 4.f)
        {
            glUniform1f(opacity_id, 1.f);
        }
        else
        {
            glUniform1f(opacity_id, 1.f-(t-4.f));
        }
        glEnable(GL_BLEND);
        {
            vec ld = look_dir;
            vMulS(&ld, ld, 0.8f);
            vec np = (vec){-pp.x, -pp.y, -pp.z};
            vAdd(&np, np, ld);

            mIdent(&model);
            mSetPos(&model, np);
            mRotZ(&model, -xrot);
            mMul(&modelview, &model, &view);
            glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
            modelBind(&mdlIntro);
            glUniform3f(color_id, 0.f, 0.7f, 0.7f);
            glDrawElements(GL_TRIANGLES, intro_numind, GL_UNSIGNED_SHORT, 0);
        }
        {
            vec ld = look_dir;
            vMulS(&ld, ld, 0.8f);
            vec np = (vec){-pp.x, -pp.y, -pp.z};
            vAdd(&np, np, ld);

            vec vy = looky;
            vMulS(&vy, vy, -0.002f);
            vAdd(&np, np, vy);

            mIdent(&model);
            mSetPos(&model, np);
            mRotZ(&model, -xrot);
            mMul(&modelview, &model, &view);
            glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
            modelBind(&mdlIntro);
            glUniform3f(color_id, 0.f, 1.f, 1.f);
            glDrawElements(GL_TRIANGLES, intro_numind, GL_UNSIGNED_SHORT, 0);
        }
        glDisable(GL_BLEND);
    }

    ///

    // display render
    glfwSwapBuffers(window);
}

//*************************************
// input
//*************************************
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // control
    if(action == GLFW_PRESS)
    {
        if(     key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  { keystate[0] = 1; }
        else if(key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) { keystate[1] = 1; }
        else if(key == GLFW_KEY_W || key == GLFW_KEY_UP)    { keystate[2] = 1; }
        else if(key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  { keystate[3] = 1; }
        else if(key == GLFW_KEY_LEFT_SHIFT ||
                key == GLFW_KEY_RIGHT_CONTROL)              { keystate[4] = 1;}
        else if(key == GLFW_KEY_N) // new game
        {
            timeTaken(0);
            char strts[16];
            timestamp(&strts[0]);
            printf("[%s] Game End.\n", strts);
            printf("[%s] TUX KILLED %u - PELTS HARVESTED %u - ALIENS KILLED %u\n", strts, tuxkilled, alienpelts, alienskilled);
            printf("[%s] Time-Taken: %s or %g Seconds\n\n", strts, tts, t-st);
            newGame(time(0));
        }
        else if(key == GLFW_KEY_ESCAPE) // toggle mouse focus
        {
            focus_cursor = 0;
#ifndef WEB
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(window, &lx, &ly);
#endif
        }
        else if(key == GLFW_KEY_F) // show average fps
        {
            if(t-lfct > 2.0)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] FPS: %g\n", strts, fc/(t-lfct));
                lfct = t;
                fc = 0;
            }
        }
    }
    else if(action == GLFW_RELEASE)
    {
        if(     key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  { keystate[0] = 0; }
        else if(key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) { keystate[1] = 0; }
        else if(key == GLFW_KEY_W || key == GLFW_KEY_UP)    { keystate[2] = 0; }
        else if(key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  { keystate[3] = 0; }
        else if(key == GLFW_KEY_LEFT_SHIFT ||
                key == GLFW_KEY_RIGHT_CONTROL)              { keystate[4] = 0;}
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        if(focus_cursor == 0)
        {
            focus_cursor = 1;
#ifndef WEB
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &lx, &ly);
#endif
            return;
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            for(int i = 0; i < MAX_BULLETS; i++)
            {
                if(bullet_pos[i].z == -3.f)
                {
                    bullet_dir[i] = look_dir;
                    bullet_dir[i].z = 0.f;
                    bullet_pos[i] = barrel_pos;
                    akdist = 0.164f;
                    akreset = t+0.08f;
                    break;
                }
            }
        }
        else if(button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            akxo = 0.f;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        if(button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            akxo = -0.074f;
        }
    }
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
    winw = width, winh = height;
    glViewport(0, 0, winw, winh);
    aspect = (float)winw / (float)winh;
    ww = winw, wh = winh;
    mIdent(&projection);
    mPerspective(&projection, 60.0f, aspect, 0.001f, FAR_DISTANCE);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
}
#ifdef WEB
EM_BOOL emscripten_resize_event(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    winw = uiEvent->documentBodyClientWidth;
    winh = uiEvent->documentBodyClientHeight;
    window_size_callback(window, winw, winh);
    emscripten_set_canvas_element_size("canvas", winw, winh);
    return EM_FALSE;
}
#endif

//*************************************
// process entry point
//*************************************
int main(int argc, char** argv)
{
    // allow custom msaa level & mouse sensitivity
    int msaa = 16;
    if(argc >= 2){msaa = atoi(argv[1]);}
    if(argc >= 3){sens = atof(argv[2]);}

    // help
    printf("----\n");
    printf("James William Fletcher (github.com/mrbid) (notabug.org/Vandarin)\n");
    printf("%s - a war rages on in antarctica between penguins and hostile aliens...\n", appTitle);
    printf("----\n");
#ifndef WEB
    printf("Two command line arguments, msaa 0-16, mouse sensitivity.\n");
    char lower_name[256];
    sprintf(lower_name, "%s", appTitle);
    lower_name[0] = tolower(lower_name[0]);
    printf("e.g; ./%s 16 0.003\n", lower_name);
    printf("----\n");
#endif
    printf("ESCAPE = Unlock Mouse\n");
    printf("W,A,S,D / Arrow Keys = Move\n");
    printf("L-SHIFT / R-CTRL = Sprint\n");
    printf("Left Click = Shoot\n");
    printf("Right Click = Iron Sights\n");
    printf("----\n");
    printf("Alien made by Cuneyt Yetisenler\n");
    printf("https://cyon4d.itch.io/\n");
    printf("https://lospec.com/gallery/cyon4d/psychicer-alien-lowpolyworkshop-challenge-entry\n");
    printf("https://pixeljoint.com/p/29255.htm?sec=icons&pg=1\n");
    printf("Express permission granted via email.\n");
    printf("----\n");
    printf("Tux made by Andy Cuccaro\n");
    printf("https://sketchfab.com/3d-models/tux-157de95fa4014050a969a8361a83d366\n");
    printf("CC BY 4.0 DEED (Attribution 4.0 International)\n");
    printf("https://creativecommons.org/licenses/by/4.0/\n");
    printf("----\n");
    printf("Christmas Tree made by yaner24\n");
    printf("https://downloadfree3d.com/3d-printing-models/art/little-christmas-tree/\n");
    printf("CC BY-NC-ND 4.0 DEED (4.0 International License)\n");
    printf("https://creativecommons.org/licenses/by-nc-nd/4.0/\n");
    printf("----\n");
    printf("UFO made by Manuel Wins based on descriptions by Bob Lazar.\n");
    printf("https://downloadfree3d.com/3d-models/aircraft/spaceship/bob-lazar-ufo/\n");
    printf("----\n");
    printf("AK-47 made by AdityaAmbatkar\n");
    printf("https://downloadfree3d.com/3d-models/weapons/rifle/ak-47-6/\n");
    printf("----\n");
    printf("RamboTux image by Santang\n");
    printf("https://3.bp.blogspot.com/_OTIWl8p4ISc/SgxWz8Ixa5I/AAAAAAAABnM/hSxs5hdrya8/s1600-h/santang-rambo-tux.png\n");
    printf("https://web.archive.org/web/20151002050322/http://www.crystalxp.net/forum/en/profil-27983.htm\n");
    printf("----\n");
    printf("Font is Plastic Love by Azkarizki\n");
    printf("https://www.fontspace.com/plastic-love-font-f49676\n");
    printf("----\n");
    printf("Icon by Forest Walter\n");
    printf("https://www.forrestwalter.com/\n");
    printf("----\n");
    printf("Merry Christmas 2023! and a happy new year 2024!\n");
    printf("----\n");
    printf("%s\n", glfwGetVersionString());
    printf("----\n");

    // init glfw
    if(!glfwInit()){printf("glfwInit() failed.\n"); exit(EXIT_FAILURE);}
#ifdef WEB
    double width, height;
    emscripten_get_element_css_size("body", &width, &height);
    winw = (uint)width, winh = (uint)height;
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, msaa);
    window = glfwCreateWindow(winw, winh, appTitle, NULL, NULL);
    if(!window)
    {
        printf("glfwCreateWindow() failed.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
#ifndef WEB
    glfwSetWindowPos(window, (desktop->width/2)-(winw/2), (desktop->height/2)-(winh/2)); // center window on desktop
#endif
    if(glfwRawMouseMotionSupported() == GLFW_TRUE){glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);} // raw input, since it's an FPS
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync

    // set icon
    glfwSetWindowIcon(window, 1, &(GLFWimage){16, 16, (unsigned char*)&icon_image.pixel_data});

//*************************************
// bind vertex and index buffers
//*************************************

    // ***** BIND INTRO *****
    esBind(GL_ARRAY_BUFFER, &mdlIntro.vid, intro_vertices, sizeof(intro_vertices), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlIntro.iid, intro_indices, sizeof(intro_indices), GL_STATIC_DRAW);
    
    // ***** BIND TERRAIN *****
    esBind(GL_ARRAY_BUFFER, &mdlTerrain.vid, terrain_vertices, sizeof(terrain_vertices), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlTerrain.iid, terrain_indices, sizeof(terrain_indices), GL_STATIC_DRAW);

    // ***** BIND BULLET *****
    esBind(GL_ARRAY_BUFFER, &mdlBullet.vid, bullet_vertices, sizeof(bullet_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlBullet.nid, bullet_normals, sizeof(bullet_normals), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlBullet.iid, bullet_indices, sizeof(bullet_indices), GL_STATIC_DRAW);

    // ***** BIND BEAM *****
    esBind(GL_ARRAY_BUFFER, &mdlBeam.vid, beam_vertices, sizeof(beam_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlBeam.nid, beam_normals, sizeof(beam_normals), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlBeam.iid, beam_indices, sizeof(beam_indices), GL_STATIC_DRAW);

    // ***** BIND AK-47 *****
    esBind(GL_ARRAY_BUFFER, &mdlAK47.vid, ak47_vertices, sizeof(ak47_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAK47.nid, ak47_normals, sizeof(ak47_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAK47.cid, ak47_colors, sizeof(ak47_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlAK47.iid, ak47_indices, sizeof(ak47_indices), GL_STATIC_DRAW);

    // ***** BIND TUX *****
    esBind(GL_ARRAY_BUFFER, &mdlTux.vid, tux_vertices, sizeof(tux_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTux.nid, tux_normals, sizeof(tux_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTux.cid, tux_colors, sizeof(tux_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlTux.iid, tux_indices, sizeof(tux_indices), GL_STATIC_DRAW);

    // ***** BIND PELT *****
    esBind(GL_ARRAY_BUFFER, &mdlPelt.vid, pelt_vertices, sizeof(pelt_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlPelt.nid, pelt_normals, sizeof(pelt_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlPelt.cid, pelt_colors, sizeof(pelt_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlPelt.iid, pelt_indices, sizeof(pelt_indices), GL_STATIC_DRAW);

    // ***** BIND ICEHOLE *****
    esBind(GL_ARRAY_BUFFER, &mdlIcehole.vid, icehole_vertices, sizeof(icehole_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlIcehole.nid, icehole_normals, sizeof(icehole_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlIcehole.cid, icehole_colors, sizeof(icehole_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlIcehole.iid, icehole_indices, sizeof(icehole_indices), GL_STATIC_DRAW);

    // ***** BIND TREE *****
    esBind(GL_ARRAY_BUFFER, &mdlTree.vid, tree_vertices, sizeof(tree_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree.nid, tree_normals, sizeof(tree_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree.cid, tree_colors, sizeof(tree_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlTree.iid, tree_indices, sizeof(tree_indices), GL_STATIC_DRAW);

    // ***** BIND UFO *****
    esBind(GL_ARRAY_BUFFER, &mdlUFO.vid, ufo_vertices, sizeof(ufo_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlUFO.nid, ufo_normals, sizeof(ufo_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlUFO.cid, ufo_colors, sizeof(ufo_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlUFO.iid, ufo_indices, sizeof(ufo_indices), GL_STATIC_DRAW);

    // ***** BIND ALIEN *****
    esBind(GL_ARRAY_BUFFER, &mdlAlien.vid, alien_vertices, sizeof(alien_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAlien.nid, alien_normals, sizeof(alien_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAlien.cid, alien_colors, sizeof(alien_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlAlien.iid, alien_indices, sizeof(alien_indices), GL_STATIC_DRAW);

    // ***** BIND ALIEN PELT *****
    esBind(GL_ARRAY_BUFFER, &mdlAlienPelt.vid, alien_pelt_vertices, sizeof(alien_pelt_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAlienPelt.nid, alien_pelt_normals, sizeof(alien_pelt_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlAlienPelt.cid, alien_pelt_colors, sizeof(alien_pelt_colors), GL_STATIC_DRAW);
    esBind(GL_ELEMENT_ARRAY_BUFFER, &mdlAlienPelt.iid, alien_pelt_indices, sizeof(alien_pelt_indices), GL_STATIC_DRAW);

//*************************************
// compile & link shader programs
//*************************************
    makeFullbright();
    makeLambert1();
    makeLambert3();

//*************************************
// configure render options
//*************************************
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.745f, 0.8863f, 0.f);

    shadeLambert3(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    window_size_callback(window, winw, winh);

//*************************************
// execute update / render loop
//*************************************

    // init
    newGame(NEWGAME_SEED);
    t = (float)glfwGetTime();
    lfct = t;

#ifdef WEB
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_FALSE, emscripten_resize_event);
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(!glfwWindowShouldClose(window)){main_loop();}
#endif

    // end
    timeTaken(0);
    char strts[16];
    timestamp(&strts[0]);
    printf("[%s] Game End.\n", strts);
    printf("[%s] TUX KILLED %u - PELTS HARVESTED %u - ALIENS KILLED %u\n", strts, tuxkilled, alienpelts, alienskilled);
    printf("[%s] Time-Taken: %s or %g Seconds\n\n", strts, tts, t-st);

    // done
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}

/*
 * This file is part of OpenCorsairLink.
 * Copyright (C) 2017-2019  Sean Nelson <audiohacked@gmail.com>

 * OpenCorsairLink is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * any later version.

 * OpenCorsairLink is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with OpenCorsairLink.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "device.h"
#include "driver.h"
#include "logic/print.h"
#include "logic/options.h"
#include "lowlevel/commanderpro.h"
#include "logic/options.h"

#include <errno.h>
#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int corsairlink_commanderpro_get_led_setup_mask(
    struct corsair_device_info *dev,
    struct libusb_device_handle *handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x30;
    commands[1] = ctrl->channel;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_led_rgb_colour(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x31;
    commands[1] = ctrl->channel;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_init_led(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    int ii;
    uint8_t response[16];
    uint8_t commands[64];
    
    uint8_t types[7];
    uint8_t devices[7];
    uint8_t content1[7];
    uint8_t content2[7];

    types[0] = 0x37;
    types[1] = 0x34;
    types[2] = 0x39;
    types[3] = 0x3B;
    types[4] = 0x38;
    types[5] = 0x35;
    types[6] = 0x33;

    for (ii = 0; ii < 6; ii++) devices[ii] = 0x01;
    devices[6] = 0xFF;

    content1[0] = 0x00;
    content1[1] = 0x00;
    content1[2] = 0x64;
    content1[3] = 0x01;
    content1[4] = 0x01;
    content1[5] = 0x00;
    content1[6] = 0x00;

    for (ii = 0; ii < 7; ii++) {
        if (ii == 5) {
            content2[ii] = 0x10;
        } else {
            content2[ii] = 0x00;
        }
    }

    for (ii = 0; ii < 7; ii++) {
        memset( response, 0, sizeof( response ) );
        memset( commands, 0, sizeof( commands ) );

        commands[0] = types[ii];
        commands[1] = devices[ii];
        commands[2] = content1[ii];
        commands[3] = content2[ii];

        rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
        rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

        dump_packet( commands, sizeof( commands ) );
        dump_packet( response, sizeof( response ) );

    }
    return rr;
}

int corsairlink_commanderpro_start_led_rgb(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    uint8_t device, 
    uint8_t channel) 
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x38;
    commands[1] = device;
    commands[2] = channel;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_commit_led_rgb(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle) 
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x33;
    commands[1] = 0xFF;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_led_rgb_values(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    int col;
    int ii;
    uint8_t response[16];
    uint8_t commands[64];
    uint8_t device;
    uint8_t channel;

    device = 0x01;
    channel = 0x02;

    corsairlink_commanderpro_start_led_rgb(dev, handle, device, channel);

    for (col = 0; col < 3; col++) {
        memset( response, 0, sizeof( response ) );
        memset( commands, 0, sizeof( commands ) );

        commands[0] = 0x32;
        commands[1] = device;
        commands[2] = 0x00;
        commands[3] = 0x10;
        commands[4] = (uint8_t) col;

        for (ii = 0; ii < 16; ii++) {
            if (col == 0) {
                commands[5+ii] = ctrl->led_colors[ii].red;
            } else if (col == 1) {
                commands[5+ii] = ctrl->led_colors[ii].green;
            } else {
                commands[5+ii] = ctrl->led_colors[ii].blue;
            }
        }

        rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
        rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

        dump_packet( commands, sizeof( commands ) );
        dump_packet( response, sizeof( response ) );
    }

    corsairlink_commanderpro_commit_led_rgb(dev, handle);

    return rr;
}

int corsairlink_commanderpro_set_commit(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x33;
    commands[1] = 0xFF;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_begin(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x34;
    commands[1] = ctrl->channel;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_led_config(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0x00] = 0x35;
    commands[0x01] = ctrl->channel; // CLNP led_channel
    commands[0x02] = ctrl->count * 10;
    commands[0x03] = ctrl->led_type; // 0x0A = LED Strip, 0x0C = HD Fan, 0x01 = SP
                               // Fan, 0x04 = ML Fan
    commands[0x04] = ctrl->mode;
    commands[0x05] = ctrl->speed;
    commands[0x06] = ctrl->direction;
    commands[0x07] = ctrl->change_style;
    commands[0x08] = 0xFF;

    commands[0x09] = ctrl->led_colors[0].red;
    commands[0x0A] = ctrl->led_colors[0].green;
    commands[0x0B] = ctrl->led_colors[0].blue;

    commands[0x0C] = ctrl->led_colors[1].red;
    commands[0x0D] = ctrl->led_colors[1].green;
    commands[0x0E] = ctrl->led_colors[1].blue;

    commands[0x0F] = ctrl->led_colors[2].red;
    commands[0x10] = ctrl->led_colors[2].green;
    commands[0x11] = ctrl->led_colors[2].blue;

    commands[0x12] = ( ctrl->temperatures[0] >> 8 );
    commands[0x13] = ( ctrl->temperatures[0] && 0xFF );

    commands[0x14] = ( ctrl->temperatures[1] >> 8 );
    commands[0x15] = ( ctrl->temperatures[1] && 0xFF );

    commands[0x16] = ( ctrl->temperatures[2] >> 8 );
    commands[0x17] = ( ctrl->temperatures[2] && 0xFF );

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_led_temperature(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x36;
    commands[1] = ctrl->channel;
    commands[2] = ctrl->channel;
    commands[3] = 0x0A;
    commands[4] = 0x28;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_led_group_reset(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x37;
    commands[1] = ctrl->channel;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_port_state(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x38;
    commands[1] = ctrl->channel;
    commands[2] = 0x01;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_port_brightness(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x39;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_port_led_count(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x3A;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

int corsairlink_commanderpro_set_port_protocol(
    struct corsair_device_info* dev,
    struct libusb_device_handle* handle,
    struct led_control *ctrl )
{
    int rr;
    uint8_t response[16];
    uint8_t commands[64];
    memset( response, 0, sizeof( response ) );
    memset( commands, 0, sizeof( commands ) );

    commands[0] = 0x3B;

    rr = dev->lowlevel->write( handle, dev->write_endpoint, commands, 64 );
    rr = dev->lowlevel->read( handle, dev->read_endpoint, response, 16 );

    dump_packet( commands, sizeof( commands ) );
    dump_packet( response, sizeof( response ) );
    return rr;
}

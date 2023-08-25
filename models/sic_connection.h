/*
 *  sic_connection.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SIC_CONNECTION_H
#define SIC_CONNECTION_H

#include "connection.h"

namespace nest
{

/* BeginUserDocs: synapse, astrocyte

Short description
+++++++++++++++++

Synapse type for astrocyte-neuron connections

Description
+++++++++++

``sic_connection`` connects an astrocyte to a target neuron. It sends
SICEvent from the astrocyte to the neuron to model a continuous current
input to the neuron, the slow inward current (SIC). The amplitude of the current
is the product of the astrocytic output and the weight of the
``sic_connection``.

The source node of ``sic_connection`` should be an astrocyte, and the target
node should be a neuron that handles SICEvent. A delay can be set for this
connection type.

Sends
+++++

SICEvent

See also
++++++++

astrocyte_lr_1994, aeif_cond_alpha_astro, gap_junction

EndUserDocs */

template < typename targetidentifierT >
class SICConnection : public Connection< targetidentifierT >
{

public:
  // this line determines which common properties to use
  typedef CommonSynapseProperties CommonPropertiesType;
  typedef Connection< targetidentifierT > ConnectionBase;
  typedef SICEvent EventType;
  static constexpr ConnectionModelProperties properties = ConnectionModelProperties::HAS_DELAY;

  /**
   * Default Constructor.
   * Sets default values for all parameters. Needed by GenericConnectorModel.
   */
  SICConnection()
    : ConnectionBase()
    , weight_( 1.0 )
  {
  }

  SecondaryEvent* get_secondary_event();

  // Explicitly declare all methods inherited from the dependent base
  // ConnectionBase. This avoids explicit name prefixes in all places these
  // functions are used. Since ConnectionBase depends on the template parameter,
  // they are not automatically found in the base class.
  using ConnectionBase::get_delay_steps;
  using ConnectionBase::get_rport;
  using ConnectionBase::get_target;

  void
  check_connection( Node& s, Node& t, size_t receptor_type, const CommonPropertiesType& )
  {
    EventType ge;

    s.sends_secondary_event( ge );
    ge.set_sender( s );
    Connection< targetidentifierT >::target_.set_rport( t.handles_test_event( ge, receptor_type ) );
    Connection< targetidentifierT >::target_.set_target( &t );
  }

  /**
   * Send an event to the receiver of this connection.
   * \param e The event to send
   * \param p The port under which this connection is stored in the Connector.
   */
  void
  send( Event& e, size_t t, const CommonSynapseProperties& )
  {
    e.set_weight( weight_ );
    e.set_delay_steps( get_delay_steps() );
    e.set_receiver( *get_target( t ) );
    e.set_rport( get_rport() );
    e();
  }

  void get_status( DictionaryDatum& d ) const;

  void set_status( const DictionaryDatum& d, ConnectorModel& cm );

  void
  set_weight( double w )
  {
    weight_ = w;
  }

  // void
  // set_delay( double )
  // {
  //   throw BadProperty( "sic_connection connection has no delay" );
  // }

private:
  double weight_; //!< connection weight
};

template < typename targetidentifierT >
constexpr ConnectionModelProperties SICConnection< targetidentifierT >::properties;

template < typename targetidentifierT >
void
SICConnection< targetidentifierT >::get_status( DictionaryDatum& d ) const
{
  // We have to include the delay here to prevent
  // errors due to internal calls of
  // this function in SLI/pyNEST
  ConnectionBase::get_status( d );
  def< double >( d, names::weight, weight_ );
  def< long >( d, names::size_of, sizeof( *this ) );
}

template < typename targetidentifierT >
SecondaryEvent*
SICConnection< targetidentifierT >::get_secondary_event()
{
  return new SICEvent();
}

template < typename targetidentifierT >
void
SICConnection< targetidentifierT >::set_status( const DictionaryDatum& d, ConnectorModel& cm )
{
  // If the delay is set, we throw a BadProperty
  // if ( d->known( names::delay ) )
  // {
  //   throw BadProperty( "sic_connection connection has no delay" );
  // }

  ConnectionBase::set_status( d, cm );
  updateValue< double >( d, names::weight, weight_ );
}

} // namespace

#endif /* #ifndef SIC_CONNECTION_H */
